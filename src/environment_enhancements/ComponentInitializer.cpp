#include "main.hpp"
#include "environment_enhancements/ComponentInitializer.hpp"
#include "environment_enhancements/EnvironmentEnhancementManager.hpp"
#include "colorizer/LightColorizer.hpp"
#include "hooks/TrackLaneRingsManager.hpp"
#include "lighting/ChromaRingsRotationEffect.hpp"

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
#include "environment_enhancements/GameObjectTrackController.hpp"


using namespace GlobalNamespace;
using namespace Chroma;

template <typename T, typename F>
requires(std::is_convertible_v<F, std::function < void(T*, T*)>>)
static void constexpr GetComponentAndOriginal(UnityEngine::Transform* root, UnityEngine::Transform* original, F&& initializeDelegate) {
    ArrayW<T*> rootComponents = root->GetComponents<T*>();
    ArrayW<T*> originalComponents = original->GetComponents<T*>();

    for (int i = 0; i < rootComponents.Length(); i++)
    {
        initializeDelegate(rootComponents.get(i), originalComponents.get(i));

        if (getChromaConfig().PrintEnvironmentEnhancementDebug.GetValue())
        {
            getLogger().info("Initialized %s", il2cpp_utils::ClassStandardName(classof(T*)).c_str());
        }
    }
}

GameObjectInfo const &
Chroma::ComponentInitializer::InitializeComponents(UnityEngine::Transform *root, UnityEngine::Transform *original,
                                                   std::vector<GameObjectInfo> &gameObjectInfos,
                                                   std::vector<std::shared_ptr<IComponentData>> &componentDatas) {
//    UnityEngine::Object::DestroyImmediate(root->GetComponent<GameObjectTrackController*>());

    GetComponentAndOriginal<LightWithIdMonoBehaviour>(root, original, [&](LightWithIdMonoBehaviour* rootComponent, LightWithIdMonoBehaviour* originalComponent) {
        rootComponent->lightManager = originalComponent->lightManager;
        LightIdRegisterer::MarkForTableRegister(rootComponent->i_ILightWithId());
    });

    GetComponentAndOriginal<LightWithIds>(root, original, [&](LightWithIds* rootComponent, LightWithIds* originalComponent) {
        rootComponent->lightManager = originalComponent->lightManager;
        // cross fingers no stripping
        auto lightsWithIdArray = System::Linq::Enumerable::ToArray(rootComponent->lightWithIds);

        for (auto const& lightIdData : lightsWithIdArray) {
            LightIdRegisterer::MarkForTableRegister(lightIdData->i_ILightWithId());
        }
    });

    GetComponentAndOriginal<TrackLaneRing>(root, original, [&](TrackLaneRing* rootComponent, TrackLaneRing* originalComponent) {
        auto ringIt = EnvironmentEnhancementManager::RingRotationOffsets.find(originalComponent);

        if (ringIt != EnvironmentEnhancementManager::RingRotationOffsets.end())
            EnvironmentEnhancementManager::RingRotationOffsets[rootComponent] = ringIt->second;

        rootComponent->transform = root;
        rootComponent->positionOffset = originalComponent->positionOffset;
        rootComponent->posZ = originalComponent->posZ;

        TrackLaneRingsManager* managerToAdd = nullptr;
        for (auto const& manager : Chroma::TrackLaneRingsManagerHolder::RingManagers) {

            std::optional<TrackLaneRingsManagerComponentData*> componentData;
            for (auto const& componentDataC : componentDatas) {
                if (componentDataC->getComponentType() == ComponentType::TrackLaneRingsManager) {
                    auto trackLaneData = std::static_pointer_cast<TrackLaneRingsManagerComponentData>(componentDataC);

                    if ((GlobalNamespace::TrackLaneRingsManager*) trackLaneData->OldTrackLaneRingsManager == manager) {
                        componentData = trackLaneData.get();
                        goto afterFindComponent;
                    }
                }
            }

            afterFindComponent:

            if (componentData) {
                managerToAdd = (GlobalNamespace::TrackLaneRingsManager*) componentData.value()->NewTrackLaneRingsManager;
            } else {
                auto rings = manager->rings;
                if (rings.Contains(originalComponent))
                    managerToAdd = manager;
            }

            if (managerToAdd) {
                auto rings = managerToAdd->rings;

                if (rings) {
                    std::vector<GlobalNamespace::TrackLaneRing *> newRingList(rings.begin(), rings.end());
                    newRingList.push_back(rootComponent);

                    managerToAdd->rings = il2cpp_utils::vectorToArray(newRingList);
                } else {
                    managerToAdd->rings = Array<GlobalNamespace::TrackLaneRing*>::New(rootComponent);
                }

                break;
            }
        }
    });

    GetComponentAndOriginal<TrackLaneRingsPositionStepEffectSpawner>(root, original, [&](TrackLaneRingsPositionStepEffectSpawner* rootComponent, TrackLaneRingsPositionStepEffectSpawner* originalComponent) {
        for (auto const& manager : TrackLaneRingsManagerHolder::RingManagers) {
            std::optional<TrackLaneRingsManagerComponentData*> componentData;
            for (auto const& componentDataC : componentDatas) {
                if (componentDataC->getComponentType() == ComponentType::TrackLaneRingsManager) {
                    auto trackLaneData = std::static_pointer_cast<TrackLaneRingsManagerComponentData>(componentDataC);

                    if ((GlobalNamespace::TrackLaneRingsManager*) trackLaneData->OldTrackLaneRingsManager == manager) {
                        componentData = trackLaneData.get();
                        break;
                    }
                }
            }

            if (componentData) {
                rootComponent->trackLaneRingsManager = (GlobalNamespace::TrackLaneRingsManager*) componentData.value()->NewTrackLaneRingsManager;

                break;
            }
        }
    });

    GetComponentAndOriginal<ChromaRingsRotationEffect>(root, original, [&](ChromaRingsRotationEffect* rootComponent, ChromaRingsRotationEffect* originalComponent)  {
        for (auto const& manager : TrackLaneRingsManagerHolder::RingManagers) {
            std::optional<TrackLaneRingsManagerComponentData*> componentData;
            for (auto const& componentDataC : componentDatas) {
                if (componentDataC->getComponentType() == ComponentType::TrackLaneRingsManager) {
                    auto trackLaneData = std::static_pointer_cast<TrackLaneRingsManagerComponentData>(componentDataC);

                    if ((GlobalNamespace::TrackLaneRingsManager*) trackLaneData->OldTrackLaneRingsManager == manager) {
                        componentData = trackLaneData.get();
                        goto doSet;
                    }
                }
            }

            doSet:
            if (componentData) {
                rootComponent->SetNewRingManager((GlobalNamespace::TrackLaneRingsManager*) componentData.value()->NewTrackLaneRingsManager);

                break;
            }
        }
    });


    GetComponentAndOriginal<TrackLaneRingsRotationEffectSpawner>(root, original, [&](TrackLaneRingsRotationEffectSpawner* rootComponent, TrackLaneRingsRotationEffectSpawner* originalComponent) constexpr {
        rootComponent->beatmapCallbacksController = originalComponent->beatmapCallbacksController;
        rootComponent->trackLaneRingsRotationEffect = rootComponent->GetComponent<ChromaRingsRotationEffect*>();
    });

    GetComponentAndOriginal<Spectrogram>(root, original, [&](Spectrogram* rootComponent, Spectrogram* originalComponent) constexpr {
        rootComponent->spectrogramData = originalComponent->spectrogramData;
    });

    GetComponentAndOriginal<LightRotationEventEffect>(root, original, [&](LightRotationEventEffect* rootComponent, LightRotationEventEffect* originalComponent) constexpr {
        rootComponent->beatmapCallbacksController = originalComponent->beatmapCallbacksController;
    });

    GetComponentAndOriginal<LightPairRotationEventEffect>(root, original, [&](LightPairRotationEventEffect* rootComponent, LightPairRotationEventEffect* originalComponent) constexpr {
        rootComponent->beatmapCallbacksController = originalComponent->beatmapCallbacksController;
        rootComponent->audioTimeSource = originalComponent->audioTimeSource;

        auto transformL = originalComponent->transformL;
        auto transformR = originalComponent->transformR;

        rootComponent->transformL = root->GetChild(transformL->GetSiblingIndex());
        rootComponent->transformR = root->GetChild(transformR->GetSiblingIndex());


        // We have to enable the object to tell unity to run Start
        rootComponent->set_enabled(true);
    });


    GetComponentAndOriginal<ParticleSystemEventEffect>(root, original, [&](ParticleSystemEventEffect* rootComponent, ParticleSystemEventEffect* originalComponent) constexpr {
        rootComponent->beatmapCallbacksController = originalComponent->beatmapCallbacksController;
        rootComponent->particleSystem = root->GetComponent<UnityEngine::ParticleSystem*>();

        rootComponent->set_enabled(true);
    });

    GetComponentAndOriginal<Mirror>(root, original, [&](Mirror* rootComponent, Mirror* originalComponent) constexpr {
        rootComponent->mirrorRenderer = UnityEngine::Object::Instantiate(originalComponent->mirrorRenderer);
        rootComponent->mirrorMaterial = UnityEngine::Object::Instantiate(originalComponent->mirrorMaterial);
    });

    auto const& newGameObject = gameObjectInfos.emplace_back(root->get_gameObject());

    auto rootChildCount = root->get_childCount();

    for (int i = 0; i < rootChildCount; i++)
    {
        auto transform = root->GetChild(i);

        int index = transform->GetSiblingIndex();
        InitializeComponents(transform, original->GetChild(index), gameObjectInfos, componentDatas);
    }

    return newGameObject;
}

void
ComponentInitializer::PrefillComponentsData(UnityEngine::Transform *root, std::vector<std::shared_ptr<IComponentData>>& componentDatas) {
    auto *trackLaneRingsManager = root->GetComponent<GlobalNamespace::TrackLaneRingsManager *>();
    if (trackLaneRingsManager != nullptr) {
        std::shared_ptr<TrackLaneRingsManagerComponentData> manager = std::make_shared<TrackLaneRingsManagerComponentData>();
        manager->OldTrackLaneRingsManager.emplace(trackLaneRingsManager);
        componentDatas.emplace_back(manager);
    }

    componentDatas.reserve(componentDatas.size() + root->get_childCount());
    auto rootChildCount = root->get_childCount();
    for (int i = 0; i < rootChildCount; i++) {
        auto transform = root->GetChild(i);
        PrefillComponentsData(transform, componentDatas);
    }
}

void
ComponentInitializer::PostfillComponentsData(UnityEngine::Transform *root, UnityEngine::Transform* original, std::vector<std::shared_ptr<IComponentData>> const& componentDatas) {
    auto trackLaneRingsManager = root->GetComponent<GlobalNamespace::TrackLaneRingsManager*>();
    if (trackLaneRingsManager != nullptr)
    {
        auto originalManager = original->GetComponent<TrackLaneRingsManager*>();

        for (auto const& componentData : componentDatas) {
            if (componentData->getComponentType() == ComponentType::TrackLaneRingsManager) {
                auto trackLaneData = std::static_pointer_cast<TrackLaneRingsManagerComponentData>(componentData);

                if ((GlobalNamespace::TrackLaneRingsManager*) trackLaneData->OldTrackLaneRingsManager == originalManager) {
                    trackLaneData->NewTrackLaneRingsManager.emplace(trackLaneRingsManager);
                }
            }
        }
    }

    auto rotationEffect = root->GetComponent<GlobalNamespace::TrackLaneRingsRotationEffect*>();
    if (rotationEffect != nullptr)
    {
        UnityEngine::Object::Destroy(rotationEffect);
    }

    auto rootChildCount = root->get_childCount();
    for (int i = 0; i < rootChildCount; i++) {
        auto transform = root->GetChild(i);
        auto index = transform->GetSiblingIndex();
        PostfillComponentsData(transform, original->GetChild(index), componentDatas);
    }
}

void ComponentInitializer::InitializeLights(UnityEngine::GameObject *go, rapidjson::Value const &data, bool v2) {
    std::vector<ILightWithId*> lightWithIds;

    auto someLightWithIds = go->GetComponents<LightWithIds*>();
    for (auto const& n : someLightWithIds) {
        if (!n->lightWithIds) continue;

        auto enumerator = n->lightWithIds->GetEnumerator();

        // MEMORY LEAK YAY
        // TODO: Fix
        //        auto dispose = bs_hook::Disposable(enumerator->i_IDisposable());

        while (enumerator->i_IEnumerator()->MoveNext()) {
            auto e = enumerator->get_Current();
            lightWithIds.emplace_back(e->i_ILightWithId());
        }
    }

    auto otherLights = go->GetComponents<LightWithIdMonoBehaviour*>();
    for (auto const& n : otherLights) {lightWithIds.emplace_back(n->i_ILightWithId());};

    if (lightWithIds.empty()) return;

    auto lightID = ChromaUtils::getIfExists<int>(data, v2 ? NewConstants::V2_LIGHT_ID : NewConstants::LIGHT_ID);
    auto type = ChromaUtils::getIfExists<int>(data, NewConstants::LIGHT_TYPE);

    if (!type && !lightID) {
        return;
    }

    auto SetType = [&](auto&& lightWithId) {
        if (!type) {
            return;
        }

        int lightId = LightColorizer::GetLightColorizer(*type)->_lightSwitchEventEffect->lightsID;

        auto monoBehaviourCast = il2cpp_utils::try_cast<LightWithIdMonoBehaviour>(lightWithId);

        if (monoBehaviourCast) {
            monoBehaviourCast.value()->_ID = *lightID;
        } else {
            auto lightWithIdsCast = il2cpp_utils::try_cast<LightWithIds::LightWithId>(lightWithId);

            if (lightWithIdsCast) {
                lightWithIdsCast.value()->lightId = lightId;
            }
        }
    };

    auto SetLightID = [&](auto&& lightWithId) {
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
