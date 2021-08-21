#include "main.hpp"
#include "lighting/environment_enhancements/ComponentInitializer.hpp"
#include "lighting/environment_enhancements/EnvironmentEnhancementManager.hpp"
#include "colorizer/LightColorizer.hpp"
#include "hooks/TrackLaneRingsManager.hpp"
#include "lighting/ChromaRingsRotationEffect.hpp"

#include "GlobalNamespace/LightWithIdMonoBehaviour.hpp"
#include "GlobalNamespace/LightWithIds.hpp"
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

#include <functional>


using namespace GlobalNamespace;
using namespace Chroma;

template <typename T>
void GetComponentAndOriginal(UnityEngine::Transform*& root, UnityEngine::Transform*& original, std::function < void(T*, T*)> initializeDelegate) {
    Array<T*>* rootComponents = root->GetComponents<T*>();
    Array<T*>* originalComponents = original->GetComponents<T*>();

    for (int i = 0; i < rootComponents->Length(); i++)
    {
        initializeDelegate(rootComponents->get(i), originalComponents->get(i));

        if (getChromaConfig().PrintEnvironmentEnhancementDebug.GetValue())
        {
            getLogger().info("Initialized %s", il2cpp_utils::ClassStandardName(classof(T*)).c_str());
        }
    }
}

void
Chroma::ComponentInitializer::InitializeComponents(UnityEngine::Transform *root, UnityEngine::Transform *original, std::vector<GameObjectInfo>& gameObjectInfos, std::vector<std::shared_ptr<IComponentData>>& componentDatas, std::optional<int>& lightId) {
    GetComponentAndOriginal<LightWithIdMonoBehaviour>(root, original, [=](LightWithIdMonoBehaviour* rootComponent, LightWithIdMonoBehaviour* originalComponent) {
        rootComponent->lightManager = originalComponent->lightManager;
        LightColorizer::RegisterLight(rootComponent, lightId);
    });

    GetComponentAndOriginal<LightWithIds>(root, original, [=](LightWithIds* rootComponent, LightWithIds* originalComponent) {
        rootComponent->lightManager = originalComponent->lightManager;
        LightColorizer::RegisterLight(rootComponent, lightId);
    });

    GetComponentAndOriginal<TrackLaneRing>(root, original, [=](TrackLaneRing* rootComponent, TrackLaneRing* originalComponent) {
        auto ringIt = EnvironmentEnhancementManager::RingRotationOffsets.find(originalComponent);

        if (ringIt != EnvironmentEnhancementManager::RingRotationOffsets.end())
            EnvironmentEnhancementManager::RingRotationOffsets[rootComponent] = ringIt->second;

        rootComponent->transform = root;
        rootComponent->positionOffset = originalComponent->positionOffset;
        rootComponent->posZ = originalComponent->posZ;

        TrackLaneRingsManager* managerToAdd;
        for (auto& manager : Chroma::TrackLaneRingsManagerHolder::RingManagers) {

            std::optional<std::shared_ptr<TrackLaneRingsManagerComponentData>> componentData;
            for (auto& componentDataC : componentDatas) {
                if (componentDataC->getComponentType() == ComponentType::TrackLaneRingsManager) {
                    auto trackLaneData = std::static_pointer_cast<TrackLaneRingsManagerComponentData>(componentDataC);

                    if (trackLaneData->OldTrackLaneRingsManager == manager) {
                        componentData = std::make_optional(trackLaneData);
                        break;
                    }
                }
            }

            if (componentData) {
                managerToAdd = componentData.value()->NewTrackLaneRingsManager;
            } else {
                auto managerRef = manager;

                auto rings = managerRef->rings;
                if (rings->Contains(originalComponent))
                    managerToAdd = manager;
            }

            if (managerToAdd) {
                auto rings = managerToAdd->rings;

                std::vector<GlobalNamespace::TrackLaneRing *> newRingList(rings->Length() + 1); // + 1 for the element we'll add
                rings->copy_to(newRingList);
                newRingList.push_back(originalComponent);
                auto newRingArray = il2cpp_utils::vectorToArray(newRingList);
                managerToAdd->rings = newRingArray;
                break;
            }
        }
    });

    GetComponentAndOriginal<TrackLaneRingsPositionStepEffectSpawner>(root, original, [=](TrackLaneRingsPositionStepEffectSpawner* rootComponent, TrackLaneRingsPositionStepEffectSpawner* originalComponent) {
        for (auto& manager : TrackLaneRingsManagerHolder::RingManagers) {
            std::optional<std::shared_ptr<TrackLaneRingsManagerComponentData>> componentData;
            for (auto& componentDataC : componentDatas) {
                if (componentDataC->getComponentType() == ComponentType::TrackLaneRingsManager) {
                    auto trackLaneData = std::static_pointer_cast<TrackLaneRingsManagerComponentData>(componentDataC);

                    if (trackLaneData->OldTrackLaneRingsManager == manager) {
                        componentData = std::make_optional(trackLaneData);
                        break;
                    }
                }
            }

            if (componentData) {
                rootComponent->trackLaneRingsManager = componentData.value()->NewTrackLaneRingsManager;

                break;
            }
        }
    });

    GetComponentAndOriginal<ChromaRingsRotationEffect>(root, original, [=](ChromaRingsRotationEffect* rootComponent, ChromaRingsRotationEffect* originalComponent) {
        for (auto& manager : TrackLaneRingsManagerHolder::RingManagers) {
            std::optional<std::shared_ptr<TrackLaneRingsManagerComponentData>> componentData;
            for (auto& componentDataC : componentDatas) {
                if (componentDataC->getComponentType() == ComponentType::TrackLaneRingsManager) {
                    auto trackLaneData = std::static_pointer_cast<TrackLaneRingsManagerComponentData>(componentDataC);

                    if (trackLaneData->OldTrackLaneRingsManager == manager) {
                        componentData = std::make_optional(trackLaneData);
                        break;
                    }
                }
            }

            if (componentData) {
                rootComponent->SetNewRingManager(componentData.value()->NewTrackLaneRingsManager);

                break;
            }
        }
    });


    GetComponentAndOriginal<TrackLaneRingsRotationEffectSpawner>(root, original, [=](TrackLaneRingsRotationEffectSpawner* rootComponent, TrackLaneRingsRotationEffectSpawner* originalComponent) {
        rootComponent->beatmapObjectCallbackController = originalComponent->beatmapObjectCallbackController;
        rootComponent->trackLaneRingsRotationEffect = rootComponent->GetComponent<ChromaRingsRotationEffect*>();
    });

    GetComponentAndOriginal<Spectrogram>(root, original, [=](Spectrogram* rootComponent, Spectrogram* originalComponent) {
        rootComponent->spectrogramData = originalComponent->spectrogramData;
    });

    GetComponentAndOriginal<LightRotationEventEffect>(root, original, [=](LightRotationEventEffect* rootComponent, LightRotationEventEffect* originalComponent) {
        rootComponent->beatmapObjectCallbackController = originalComponent->beatmapObjectCallbackController;
    });

    GetComponentAndOriginal<LightPairRotationEventEffect>(root, original, [=](LightPairRotationEventEffect* rootComponent, LightPairRotationEventEffect* originalComponent) {
        rootComponent->beatmapObjectCallbackController = originalComponent->beatmapObjectCallbackController;

        auto transformL = originalComponent->transformL;
        auto transformR = originalComponent->transformR;

        rootComponent->transformL = root->GetChild(transformL->GetSiblingIndex());
        rootComponent->transformR = root->GetChild(transformR->GetSiblingIndex());


        // We have to enable the object to tell unity to run Start
        rootComponent->set_enabled(true);
    });


    GetComponentAndOriginal<ParticleSystemEventEffect>(root, original, [=](ParticleSystemEventEffect* rootComponent, ParticleSystemEventEffect* originalComponent) {
        rootComponent->beatmapObjectCallbackController = originalComponent->beatmapObjectCallbackController;
        rootComponent->particleSystem = root->GetComponent<UnityEngine::ParticleSystem*>();

        rootComponent->set_enabled(true);
    });

    GetComponentAndOriginal<Mirror>(root, original, [=](Mirror* rootComponent, Mirror* originalComponent) {
        rootComponent->mirrorRenderer = UnityEngine::Object::Instantiate(originalComponent->mirrorRenderer);
        rootComponent->mirrorMaterial = UnityEngine::Object::Instantiate(originalComponent->mirrorMaterial);
    });

    GameObjectInfo gameObjectInfo(root->get_gameObject());
    gameObjectInfos.push_back(gameObjectInfo);

    for (int i = 0; i < root->get_childCount(); i++)
    {
        auto transform = root->GetChild(i);

        int index = transform->GetSiblingIndex();
        InitializeComponents(transform, original->GetChild(index), gameObjectInfos, componentDatas, lightId);
    }
}

void
ComponentInitializer::PrefillComponentsData(UnityEngine::Transform *root, std::vector<std::shared_ptr<IComponentData>>& componentDatas) {
    SkipAwake = true;

    auto *trackLaneRingsManager = root->GetComponent<GlobalNamespace::TrackLaneRingsManager *>();
    if (trackLaneRingsManager != nullptr) {
        std::shared_ptr<TrackLaneRingsManagerComponentData> manager{};
        manager->OldTrackLaneRingsManager = trackLaneRingsManager;
        componentDatas.push_back(manager);
    }

    componentDatas.reserve(componentDatas.size() + root->get_childCount());
    for (int i = 0; i < root->get_childCount(); i++) {
        auto transform = root->GetChild(i);
        PrefillComponentsData(transform, componentDatas);
    }
}

void
ComponentInitializer::PostfillComponentsData(UnityEngine::Transform *root, UnityEngine::Transform* original, std::vector<std::shared_ptr<IComponentData>>& componentDatas) {
    SkipAwake = false;

    auto trackLaneRingsManager = root->GetComponent<GlobalNamespace::TrackLaneRingsManager*>();
    if (trackLaneRingsManager != nullptr)
    {
        auto originalManager = original->GetComponent<TrackLaneRingsManager*>();

        for (auto& componentData : componentDatas) {
            if (componentData->getComponentType() == ComponentType::TrackLaneRingsManager) {
                auto trackLaneData = std::static_pointer_cast<TrackLaneRingsManagerComponentData>(componentData);

                if (trackLaneData->OldTrackLaneRingsManager == originalManager) {
                    trackLaneData->NewTrackLaneRingsManager = trackLaneRingsManager;
                }
            }
        }
    }

    auto rotationEffect = root->GetComponent<GlobalNamespace::TrackLaneRingsRotationEffect*>();
    if (rotationEffect != nullptr)
    {
        UnityEngine::Object::Destroy(rotationEffect);
    }


    for (int i = 0; i < root->get_childCount(); i++) {
        auto transform = root->GetChild(i);
        auto index = transform->GetSiblingIndex();
        PostfillComponentsData(transform, original->GetChild(index), componentDatas);
    }
}
