#include "main.hpp"
#include "lighting/environment_enhancements/ComponentInitializer.hpp"
#include "lighting/environment_enhancements/EnvironmentEnhancementManager.hpp"
#include "colorizer/LightColorizer.hpp"
#include "hooks/TrackLaneRingsManager.hpp"

#include "GlobalNamespace/LightWithIdMonoBehaviour.hpp"
#include "GlobalNamespace/LightWithIds.hpp"
#include "GlobalNamespace/TrackLaneRing.hpp"
#include "GlobalNamespace/TrackLaneRingsManager.hpp"
#include "GlobalNamespace/Spectrogram.hpp"
#include "GlobalNamespace/LightRotationEventEffect.hpp"
#include "GlobalNamespace/LightPairRotationEventEffect.hpp"
#include "GlobalNamespace/ParticleSystemEventEffect.hpp"
#include "GlobalNamespace/Mirror.hpp"
#include "GlobalNamespace/MirrorRendererSO.hpp"
#include "GlobalNamespace/TrackLaneRingsRotationEffect.hpp"


#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/ParticleSystem.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/Material.hpp"

#include "System/Collections/Generic/List_1.hpp"

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"


using namespace GlobalNamespace;
using namespace Chroma;

void
Chroma::ComponentInitializer::InitializeComponents(UnityEngine::Transform *root, UnityEngine::Transform *original, std::vector<GameObjectInfo> gameObjectInfos, std::vector<std::shared_ptr<IComponentData>>& componentDatas) {
    auto lightWithIdMonoBehaviour = root->GetComponent<LightWithIdMonoBehaviour*>();
    if (lightWithIdMonoBehaviour != nullptr)
    {
        auto originalLight = original->GetComponent<LightWithIdMonoBehaviour*>();
        lightWithIdMonoBehaviour->lightManager = originalLight->lightManager;
        LightColorizer::RegisterLight(lightWithIdMonoBehaviour);
    }

    auto lightWithIds = root->GetComponent<LightWithIds*>();
    if (lightWithIds != nullptr)
    {
        auto originalLight = original->GetComponent<LightWithIds*>();
        lightWithIdMonoBehaviour->lightManager = originalLight->lightManager;
        LightColorizer::RegisterLight(lightWithIds);
    }

    auto trackLaneRing = root->GetComponent<TrackLaneRing*>();
    if (trackLaneRing != nullptr)
    {
        auto originalRing = original->GetComponent<TrackLaneRing*>();

        auto ringIt = EnvironmentEnhancementManager::RingRotationOffsets.find(originalRing);

        if (ringIt != EnvironmentEnhancementManager::RingRotationOffsets.end())
            EnvironmentEnhancementManager::RingRotationOffsets[trackLaneRing] = ringIt->second;


        TrackLaneRingsManager* managerToAdd;
        for (auto& manager : RingManagers) {

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
                if (rings->Contains(originalRing))
                    managerToAdd = manager;
            }

            if (managerToAdd) {
                auto rings = managerToAdd->rings;


                System::Collections::Generic::List_1<GlobalNamespace::TrackLaneRing *> *newRingList = System::Collections::Generic::List_1<GlobalNamespace::TrackLaneRing *>::New_ctor();

                if (rings) {
                    for (int i = 0; i < rings->Length(); i++) {
                        newRingList->Add(rings->values[i]);
                    }
                }

                newRingList->Add(trackLaneRing);

                managerToAdd->rings = newRingList->ToArray();

                if (getChromaConfig().PrintEnvironmentEnhancementDebug.GetValue()) {
                    getLogger().info("Initialized TrackLaneRing");
                }

                break;

            }
        }
    }

    auto spectrogram = root->GetComponent<GlobalNamespace::Spectrogram*>();
    if (spectrogram != nullptr)
    {
        auto originalSpectrogram = original->GetComponent<GlobalNamespace::Spectrogram*>();

        spectrogram->spectrogramData = originalSpectrogram->spectrogramData;


        if (getChromaConfig().PrintEnvironmentEnhancementDebug.GetValue()) {
            getLogger().info("Initialized Spectrogram");
        }
    }

    auto lightRotationEvent = root->GetComponent<LightRotationEventEffect*>();
    if (lightRotationEvent != nullptr)
    {
        auto originalLightRotationEvent = original->GetComponent<LightRotationEventEffect*>();

        lightRotationEvent->beatmapObjectCallbackController = originalLightRotationEvent->beatmapObjectCallbackController;

        if (getChromaConfig().PrintEnvironmentEnhancementDebug.GetValue()) {
            getLogger().info("Initialized LightRotationEventEffect");
        }
    }

    auto lightPairRotationEvent = root->GetComponent<LightPairRotationEventEffect*>();
    if (lightPairRotationEvent != nullptr)
    {
        auto originalLightPairRotationEvent = original->GetComponent<LightPairRotationEventEffect*>();

        lightPairRotationEvent->beatmapObjectCallbackController = originalLightPairRotationEvent->beatmapObjectCallbackController;

        auto transformL = originalLightPairRotationEvent->transformL;
        auto transformR = originalLightPairRotationEvent->transformR;

        lightPairRotationEvent->transformL = root->GetChild(transformL->GetSiblingIndex());
        lightPairRotationEvent->transformR = root->GetChild(transformR->GetSiblingIndex());


        // We have to enable the object to tell unity to run Start
        lightPairRotationEvent->set_enabled(true);


        if (getChromaConfig().PrintEnvironmentEnhancementDebug.GetValue()) {
            getLogger().info("Initialized LightPairRotationEventEffect");
        }
    }

    auto particleSystemEvent = root->GetComponent<ParticleSystemEventEffect*>();
    if (particleSystemEvent != nullptr)
    {
        auto originalParticleSystemEvent = original->GetComponent<ParticleSystemEventEffect*>();

        particleSystemEvent->beatmapObjectCallbackController = originalParticleSystemEvent->beatmapObjectCallbackController;
        particleSystemEvent->particleSystem = root->GetComponent<UnityEngine::ParticleSystem*>();

        particleSystemEvent->set_enabled(true);

        if (getChromaConfig().PrintEnvironmentEnhancementDebug.GetValue()) {
            getLogger().info("Initialized ParticleSystemEventEffect");
        }
    }

    auto mirror = root->GetComponent<Mirror*>();
    if (mirror != nullptr)
    {
        mirror->mirrorRenderer = UnityEngine::Object::Instantiate(mirror->mirrorRenderer);
        mirror->mirrorMaterial = UnityEngine::Object::Instantiate(mirror->mirrorMaterial);


        if (getChromaConfig().PrintEnvironmentEnhancementDebug.GetValue()) {
            getLogger().info("Initialized Mirror");
        }
    }

    GameObjectInfo gameObjectInfo = GameObjectInfo(root->get_gameObject());
    gameObjectInfos.push_back(gameObjectInfo);

    for (int i = 0; i < root->get_childCount(); i++)
    {
        auto transform = root->GetChild(i);

        int index = transform->GetSiblingIndex();
        InitializeComponents(transform, original->GetChild(index), gameObjectInfos, componentDatas);
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
