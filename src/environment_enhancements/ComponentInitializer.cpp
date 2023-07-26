#include "main.hpp"
#include "environment_enhancements/ComponentInitializer.hpp"
#include "environment_enhancements/EnvironmentEnhancementManager.hpp"
#include "colorizer/LightColorizer.hpp"
#include "hooks/TrackLaneRingsManager.hpp"
#include "lighting/ChromaRingsRotationEffect.hpp"
#include "lighting/ChromaFogController.hpp"

#include "GlobalNamespace/LightWithIdMonoBehaviour.hpp"
#include "GlobalNamespace/LightWithIds.hpp"
#include "GlobalNamespace/LightWithIds_LightWithId.hpp"
#include "GlobalNamespace/ILightWithId.hpp"
#include "GlobalNamespace/TrackLaneRing.hpp"
#include "GlobalNamespace/TrackLaneRingsManager.hpp"
#include "GlobalNamespace/TrackLaneRingsPositionStepEffectSpawner.hpp"
#include "GlobalNamespace/TrackLaneRingsRotationEffectSpawner.hpp"
#include "GlobalNamespace/TrackLaneRingsRotationEffect.hpp"
#include "GlobalNamespace/Spectrogram.hpp"
#include "GlobalNamespace/LightRotationEventEffect.hpp"
#include "GlobalNamespace/LightPairRotationEventEffect.hpp"
#include "GlobalNamespace/ParticleSystemEventEffect.hpp"
#include "GlobalNamespace/Mirror.hpp"
#include "GlobalNamespace/MirrorRendererSO.hpp"
#include "GlobalNamespace/BloomFogEnvironment.hpp"
#include "GlobalNamespace/BloomFogEnvironmentParams.hpp"

#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/ParticleSystem.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/Material.hpp"

#include "System/Collections/Generic/List_1.hpp"

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "System/Linq/Enumerable.hpp"
#include "hooks/LightWithIdManager.hpp"
#include "utils/ChromaUtils.hpp"

#include <functional>

#include "beatsaber-hook/shared/utils/typedefs-disposal.hpp"

#include "sombrero/shared/linq_functional.hpp"

using namespace GlobalNamespace;
using namespace Chroma;
using namespace Sombrero::Linq::Functional;

template <typename T, typename F>
requires(std::is_convertible_v<F, std::function<void(T*, T*)>>) static void constexpr GetComponentAndOriginal(
    UnityEngine::Transform* root, UnityEngine::Transform* original, F&& initializeDelegate) {
  ArrayW<T*> rootComponents = root->GetComponents<T*>();
  ArrayW<T*> originalComponents = original->GetComponents<T*>();

  for (int i = 0; i < rootComponents.Length(); i++) {
    initializeDelegate(rootComponents.get(i), originalComponents.get(i));

    if (getChromaConfig().PrintEnvironmentEnhancementDebug.GetValue()) {
      getLogger().info("Initialized %s", il2cpp_utils::ClassStandardName(classof(T*)).c_str());
    }
  }
}

GameObjectInfo const&
Chroma::ComponentInitializer::InitializeComponents(UnityEngine::Transform* root, UnityEngine::Transform* original,
                                                   std::vector<GameObjectInfo>& gameObjectInfos,
                                                   std::vector<std::shared_ptr<IComponentData>>& componentDatas) {
  //    UnityEngine::Object::DestroyImmediate(root->GetComponent<GameObjectTrackController*>());

  GetComponentAndOriginal<LightWithIdMonoBehaviour>(
      root, original, [&](LightWithIdMonoBehaviour* rootComponent, LightWithIdMonoBehaviour* originalComponent) {
        rootComponent->lightManager = originalComponent->lightManager;
        LightIdRegisterer::MarkForTableRegister(rootComponent->i_ILightWithId());
      });

  GetComponentAndOriginal<LightWithIds>(root, original,
                                        [&](LightWithIds* rootComponent, LightWithIds* originalComponent) {
                                          rootComponent->lightManager = originalComponent->lightManager;
                                          // cross fingers no stripping
                                          if (rootComponent->lightWithIds != nullptr) {
                                            auto* lightsWithIdArray = rootComponent->lightWithIds->GetEnumerator();

                                            while (lightsWithIdArray->i_IEnumerator()->MoveNext()) {
                                              auto* lightIdData = lightsWithIdArray->get_Current();
                                              LightIdRegisterer::MarkForTableRegister(lightIdData->i_ILightWithId());
                                            }

                                            // TODO: Handle
                                            //            lightsWithIdArray->i_IDisposable()->Dispose();
                                          }
                                        });

  GetComponentAndOriginal<TrackLaneRing>(
      root, original, [&](TrackLaneRing* rootComponent, TrackLaneRing* originalComponent) {
        auto ringIt = EnvironmentEnhancementManager::RingRotationOffsets.find(originalComponent);

        if (ringIt != EnvironmentEnhancementManager::RingRotationOffsets.end()) {
          EnvironmentEnhancementManager::RingRotationOffsets[rootComponent] = ringIt->second;
        }

        rootComponent->transform = root;
        rootComponent->positionOffset = originalComponent->positionOffset;
        rootComponent->posZ = originalComponent->posZ;

        TrackLaneRingsManager* managerToAdd = nullptr;
        for (auto const& manager : Chroma::TrackLaneRingsManagerHolder::RingManagers) {

          std::optional<TrackLaneRingsManagerComponentData*> componentData;
          for (auto const& componentDataC : componentDatas) {
            if (componentDataC->getComponentType() == ComponentType::TrackLaneRingsManager) {
              auto trackLaneData = std::static_pointer_cast<TrackLaneRingsManagerComponentData>(componentDataC);

              if ((GlobalNamespace::TrackLaneRingsManager*)trackLaneData->OldTrackLaneRingsManager == manager) {
                componentData = trackLaneData.get();
                goto afterFindComponent;
              }
            }
          }

        afterFindComponent:

          if (componentData) {
            managerToAdd = (GlobalNamespace::TrackLaneRingsManager*)componentData.value()->NewTrackLaneRingsManager;
          } else {
            auto rings = manager->rings;
            if (rings.Contains(originalComponent)) {
              managerToAdd = manager;
            }
          }

          if (managerToAdd != nullptr) {
            auto rings = managerToAdd->rings;

            if (rings) {
              std::vector<GlobalNamespace::TrackLaneRing*> newRingList(rings.begin(), rings.end());
              newRingList.push_back(rootComponent);

              managerToAdd->rings = il2cpp_utils::vectorToArray(newRingList);
            } else {
              managerToAdd->rings = Array<GlobalNamespace::TrackLaneRing*>::New(rootComponent);
            }

            break;
          }
        }
      });

  GetComponentAndOriginal<TrackLaneRingsPositionStepEffectSpawner>(
      root, original,
      [&](TrackLaneRingsPositionStepEffectSpawner* rootComponent,
          TrackLaneRingsPositionStepEffectSpawner* /*originalComponent*/) {
        for (auto const& manager : TrackLaneRingsManagerHolder::RingManagers) {
          std::optional<TrackLaneRingsManagerComponentData*> componentData;
          for (auto const& componentDataC : componentDatas) {
            if (componentDataC->getComponentType() == ComponentType::TrackLaneRingsManager) {
              auto trackLaneData = std::static_pointer_cast<TrackLaneRingsManagerComponentData>(componentDataC);

              if ((GlobalNamespace::TrackLaneRingsManager*)trackLaneData->OldTrackLaneRingsManager == manager) {
                componentData = trackLaneData.get();
                break;
              }
            }
          }

          if (componentData) {
            rootComponent->trackLaneRingsManager =
                (GlobalNamespace::TrackLaneRingsManager*)componentData.value()->NewTrackLaneRingsManager;

            break;
          }
        }
      });

  GetComponentAndOriginal<ChromaRingsRotationEffect>(
      root, original, [&](ChromaRingsRotationEffect* rootComponent, ChromaRingsRotationEffect* /*originalComponent*/) {
        for (auto const& manager : TrackLaneRingsManagerHolder::RingManagers) {
          std::optional<TrackLaneRingsManagerComponentData*> componentData;
          for (auto const& componentDataC : componentDatas) {
            if (componentDataC->getComponentType() == ComponentType::TrackLaneRingsManager) {
              auto trackLaneData = std::static_pointer_cast<TrackLaneRingsManagerComponentData>(componentDataC);

              if ((GlobalNamespace::TrackLaneRingsManager*)trackLaneData->OldTrackLaneRingsManager == manager) {
                componentData = trackLaneData.get();
                goto doSet;
              }
            }
          }

        doSet:
          if (componentData) {
            rootComponent->SetNewRingManager(
                (GlobalNamespace::TrackLaneRingsManager*)componentData.value()->NewTrackLaneRingsManager);

            break;
          }
        }
      });

  GetComponentAndOriginal<TrackLaneRingsRotationEffectSpawner>(
      root, original,
      [&](TrackLaneRingsRotationEffectSpawner * rootComponent,
          TrackLaneRingsRotationEffectSpawner * originalComponent) constexpr {
        rootComponent->beatmapCallbacksController = originalComponent->beatmapCallbacksController;
        rootComponent->trackLaneRingsRotationEffect = rootComponent->GetComponent<ChromaRingsRotationEffect*>();
      });

  GetComponentAndOriginal<Spectrogram>(
      root, original, [&](Spectrogram * rootComponent, Spectrogram * originalComponent) constexpr {
        rootComponent->spectrogramData = originalComponent->spectrogramData;
      });

  GetComponentAndOriginal<LightRotationEventEffect>(
      root,
      original, [&](LightRotationEventEffect * rootComponent, LightRotationEventEffect * originalComponent) constexpr {
        rootComponent->beatmapCallbacksController = originalComponent->beatmapCallbacksController;
      });

  GetComponentAndOriginal<LightPairRotationEventEffect>(
      root, original,
      [&](LightPairRotationEventEffect * rootComponent, LightPairRotationEventEffect * originalComponent) constexpr {
        rootComponent->beatmapCallbacksController = originalComponent->beatmapCallbacksController;
        rootComponent->audioTimeSource = originalComponent->audioTimeSource;

        auto* transformL = originalComponent->transformL;
        auto* transformR = originalComponent->transformR;

        rootComponent->transformL = root->GetChild(transformL->GetSiblingIndex());
        rootComponent->transformR = root->GetChild(transformR->GetSiblingIndex());

        // We have to enable the object to tell unity to run Start
        rootComponent->set_enabled(true);
      });

  GetComponentAndOriginal<ParticleSystemEventEffect>(
      root, original,
      [&](ParticleSystemEventEffect * rootComponent, ParticleSystemEventEffect * originalComponent) constexpr {
        rootComponent->beatmapCallbacksController = originalComponent->beatmapCallbacksController;
        rootComponent->particleSystem = root->GetComponent<UnityEngine::ParticleSystem*>();

        rootComponent->set_enabled(true);
      });

  GetComponentAndOriginal<Mirror>(
      root, original, [&](Mirror * rootComponent, Mirror * originalComponent) constexpr {
        rootComponent->mirrorRenderer = UnityEngine::Object::Instantiate(originalComponent->mirrorRenderer);
        rootComponent->mirrorMaterial = UnityEngine::Object::Instantiate(originalComponent->mirrorMaterial);
      });

  auto const& newGameObject = gameObjectInfos.emplace_back(root->get_gameObject());

  auto rootChildCount = root->get_childCount();

  for (int i = 0; i < rootChildCount; i++) {
    auto* transform = root->GetChild(i);

    int index = transform->GetSiblingIndex();
    InitializeComponents(transform, original->GetChild(index), gameObjectInfos, componentDatas);
  }

  return newGameObject;
}

void ComponentInitializer::PrefillComponentsData(UnityEngine::Transform* root,
                                                 std::vector<std::shared_ptr<IComponentData>>& componentDatas) {
  auto* trackLaneRingsManager = root->GetComponent<GlobalNamespace::TrackLaneRingsManager*>();
  if (trackLaneRingsManager != nullptr) {
    std::shared_ptr<TrackLaneRingsManagerComponentData> manager =
        std::make_shared<TrackLaneRingsManagerComponentData>();
    manager->OldTrackLaneRingsManager.emplace(trackLaneRingsManager);
    componentDatas.emplace_back(manager);
  }

  componentDatas.reserve(componentDatas.size() + root->get_childCount());
  auto rootChildCount = root->get_childCount();
  for (int i = 0; i < rootChildCount; i++) {
    auto* transform = root->GetChild(i);
    PrefillComponentsData(transform, componentDatas);
  }
}

void ComponentInitializer::PostfillComponentsData(UnityEngine::Transform* root, UnityEngine::Transform* original,
                                                  std::vector<std::shared_ptr<IComponentData>> const& componentDatas) {
  auto* trackLaneRingsManager = root->GetComponent<GlobalNamespace::TrackLaneRingsManager*>();
  if (trackLaneRingsManager != nullptr) {
    Chroma::TrackLaneRingsManagerHolder::RingManagers.emplace_back(trackLaneRingsManager);
    auto* originalManager = original->GetComponent<TrackLaneRingsManager*>();

    for (auto const& componentData : componentDatas) {
      if (componentData->getComponentType() == ComponentType::TrackLaneRingsManager) {
        auto trackLaneData = std::static_pointer_cast<TrackLaneRingsManagerComponentData>(componentData);

        if ((GlobalNamespace::TrackLaneRingsManager*)trackLaneData->OldTrackLaneRingsManager == originalManager) {
          trackLaneData->NewTrackLaneRingsManager.emplace(trackLaneRingsManager);
        }
      }
    }
  }

  auto* rotationEffect = root->GetComponent<GlobalNamespace::TrackLaneRingsRotationEffect*>();
  if (rotationEffect != nullptr) {
    UnityEngine::Object::Destroy(rotationEffect);
  }

  auto rootChildCount = root->get_childCount();
  for (int i = 0; i < rootChildCount; i++) {
    auto* transform = root->GetChild(i);
    auto index = transform->GetSiblingIndex();
    PostfillComponentsData(transform, original->GetChild(index), componentDatas);
  }
}

static void InitializeTubeBloomPrePassLights(rapidjson::Value const& data, std::span<UnityEngine::Component*> comps) {
  auto tubeBloomPrePassLightJSON =
      ChromaUtils::getIfExists<rapidjson::Value::ConstObject>(data, Chroma::NewConstants::TUBE_BLOOM_PRE_PASS_LIGHT);
  if (!tubeBloomPrePassLightJSON) {
    return;
  }

  auto colorAlphaMultiplier =
      ChromaUtils::getIfExists<float>(*tubeBloomPrePassLightJSON, Chroma::NewConstants::COLOR_ALPHA_MULTIPLIER);
  auto bloomFogIntensityMultiplier =
      ChromaUtils::getIfExists<float>(*tubeBloomPrePassLightJSON, Chroma::NewConstants::BLOOM_FOG_INTENSITY_MULTIPLIER);

  auto SetColorAlphaMultiplier = [](TubeBloomPrePassLight * tubeBloomPrePassLight, float value) constexpr {
    tubeBloomPrePassLight->colorAlphaMultiplier = value;
    tubeBloomPrePassLight->MarkDirty();
  };

  for (auto const& comp : comps) {
    auto tubeCast = il2cpp_utils::try_cast<TubeBloomPrePassLight>(comp);
    if (!tubeCast) {
      continue;
    }
    auto* tubeBloomPrePassLight = *tubeCast;

    if (colorAlphaMultiplier) {
      SetColorAlphaMultiplier(tubeBloomPrePassLight, *colorAlphaMultiplier);
    }
    if (bloomFogIntensityMultiplier) {
      tubeBloomPrePassLight->bloomFogIntensityMultiplier = *bloomFogIntensityMultiplier;
    }
  }
}

static void InitializeFog(rapidjson::Value const& data, std::span<UnityEngine::Component*> comps) {
  auto bloomJSON =
      ChromaUtils::getIfExists<rapidjson::Value::ConstObject>(data, Chroma::NewConstants::BLOOM_FOG_ENVIRONMENT);

  if (!bloomJSON) {
    return;
  }

  auto attenuation = ChromaUtils::getIfExists<float>(*bloomJSON, Chroma::NewConstants::ATTENUATION);
  auto offset = ChromaUtils::getIfExists<float>(*bloomJSON, Chroma::NewConstants::OFFSET);
  auto startY = ChromaUtils::getIfExists<float>(*bloomJSON, Chroma::NewConstants::HEIGHT_FOG_STARTY);
  auto height = ChromaUtils::getIfExists<float>(*bloomJSON, Chroma::NewConstants::HEIGHT_FOG_HEIGHT);

  for (auto* comp : comps) {
    auto fogCast = il2cpp_utils::try_cast<BloomFogEnvironment>(comp);
    if (!fogCast) {
      continue;
    }

    auto* fog = *fogCast;

    auto* params = fog->fogParams;
    if (attenuation) {
      params->attenuation = Chroma::fogAttenuationFix(*attenuation);
    }
    if (offset) {
      params->offset = *offset;
    }
    if (startY) {
      params->heightFogStartY = *startY;
    }
    if (height) {
      params->heightFogHeight = *height;
    }
  }
}

static void InitializeLights(rapidjson::Value const& data, std::span<UnityEngine::Component*> comps, bool v2) {
  auto lightDataJSON =
      ChromaUtils::getIfExists<rapidjson::Value::ConstObject>(data, Chroma::NewConstants::LIGHT_WITH_ID);
  if (!v2 && !lightDataJSON) {
    return;
  }

  /// JSON
  std::optional<int> lightID;
  std::optional<int> type;
  if (v2) {
    lightID = ChromaUtils::getIfExists<int>(data, NewConstants::V2_LIGHT_ID);
  } else {
    lightID = ChromaUtils::getIfExists<int>(*lightDataJSON, NewConstants::LIGHT_ID);
    type = ChromaUtils::getIfExists<int>(*lightDataJSON, NewConstants::LIGHT_TYPE);
  }

  if (!type && !lightID) {
    return;
  }

  /// GRAB ALL LIGHT IDS
  std::vector<ILightWithId*> lightWithIds;

  for (auto const& comp : comps) {
    if (comp == nullptr) {
      continue;
    }

    if (auto castedLights = il2cpp_utils::try_cast<LightWithIds>(comp)) {
      if (castedLights.value()->lightWithIds == nullptr) {
        continue;
      }

      auto* enumerator = castedLights.value()->lightWithIds->GetEnumerator();

      // MEMORY LEAK YAY
      // TODO: Fix
      //        auto dispose = bs_hook::Disposable(enumerator->i_IDisposable());

      while (enumerator->i_IEnumerator()->MoveNext()) {
        auto* e = enumerator->get_Current();
        lightWithIds.emplace_back(e->i_ILightWithId());
      }
    } else if (auto castedMonoLight = il2cpp_utils::try_cast<LightWithIdMonoBehaviour>(comp)) {
      lightWithIds.emplace_back(castedMonoLight.value()->i_ILightWithId());
    }
  }

  if (lightWithIds.empty()) {
    return;
  }

  /// DO THE STUFF
  auto SetType = [&](auto&& lightWithId) constexpr {
    if (!type) {
      return;
    }

    int psuedoLightId = LightColorizer::GetLightColorizer(*type)->_lightSwitchEventEffect->lightsID;

    auto monoBehaviourCast = il2cpp_utils::try_cast<LightWithIdMonoBehaviour>(lightWithId);

    if (monoBehaviourCast) {
      monoBehaviourCast.value()->_ID = psuedoLightId;
    } else {
      auto lightWithIdsCast = il2cpp_utils::try_cast<LightWithIds::LightWithId>(lightWithId);

      if (lightWithIdsCast) {
        lightWithIdsCast.value()->lightId = psuedoLightId;
      }
    }
  };

  auto SetLightID = [&](auto&& lightWithId) constexpr {
    if (lightID) {
      LightIdRegisterer::SetRequestedId(lightWithId, *lightID);
    }
  };

  for (auto const& lightWithId : lightWithIds) {
    if (lightWithId->get_isRegistered()) {
      LightIdRegisterer::ForceUnregister(lightWithId);
      LightIdRegisterer::MarkForTableRegister(lightWithId);
      SetType(lightWithId);
      SetLightID(lightWithId);
      LightIdRegisterer::lightWithIdManager->RegisterLight(lightWithId);
    } else {
      SetType(lightWithId);
      SetLightID(lightWithId);
    }
  }
}

void ComponentInitializer::InitializeCustomComponents(UnityEngine::GameObject* go, rapidjson::Value const& data,
                                                      bool v2) {
  auto componentData = ChromaUtils::getIfExists<rapidjson::Value::ConstObject>(data, Chroma::NewConstants::COMPONENTS);
  if (!v2 && !componentData) {
    return;
  }

  auto comps = go->GetComponentsInChildren<UnityEngine::Component*>(true);
  SafePtr<Array<UnityEngine::Component*>> arr((Array<UnityEngine::Component*>*)comps);

  auto compsFiltered = comps | Where([](auto c) constexpr { return c && c->klass; }) | ToVector();

  //    auto compsProtect = compsFiltered | Select([](auto &&c) constexpr {
  //        return SafePtrUnity(c);
  //    }) | ToVector();

  if (v2) {
    InitializeLights(data, compsFiltered, v2);
    return;
  }
  if (!componentData) {
    return;
  }

  InitializeLights(*componentData, compsFiltered, v2);
  InitializeFog(*componentData, compsFiltered);
  InitializeTubeBloomPrePassLights(*componentData, compsFiltered);
}
