#include "lighting/environment_enhancements/ComponentInitializer.hpp"
#include "main.hpp"
#include "colorizer/LightColorizer.hpp"
#include "hooks/TrackLaneRingsManager.hpp"

#include "GlobalNamespace/LightWithIdMonoBehaviour.hpp"
#include "GlobalNamespace/LightWithIds.hpp"
#include "GlobalNamespace/TrackLaneRing.hpp"
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


using namespace GlobalNamespace;
using namespace Chroma;

void
Chroma::ComponentInitializer::InitializeComponents(UnityEngine::Transform *root, UnityEngine::Transform *original, std::vector<GameObjectInfo> gameObjectInfos) {
    auto lightWithIdMonoBehaviour = root->GetComponent<LightWithIdMonoBehaviour*>();
    if (lightWithIdMonoBehaviour != nullptr)
    {
        LightColorizer::RegisterLight(lightWithIdMonoBehaviour);
    }

    auto lightWithIds = root->GetComponent<LightWithIds*>();
    if (lightWithIds != nullptr)
    {
        LightColorizer::RegisterLight(lightWithIds);
    }

    auto trackLaneRing = root->GetComponent<TrackLaneRing*>();
    if (trackLaneRing != nullptr)
    {
        trackLaneRing->Init(UnityEngine::Vector3::get_zero(), root->get_position());

        auto originalRing = original->GetComponent<TrackLaneRing*>();

        for (auto& manager : RingManagers) {
            auto rings = manager->rings;

            if (rings->Contains(originalRing)) {
                System::Collections::Generic::List_1<GlobalNamespace::TrackLaneRing *> *newRingList = System::Collections::Generic::List_1<GlobalNamespace::TrackLaneRing *>::New_ctor();;

                for (int i = 0; i < rings->Length(); i++) {
                    newRingList->Add(rings->values[i]);
                }

                newRingList->Add(trackLaneRing);

                manager->rings = newRingList->ToArray();

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
        InitializeComponents(transform, original->GetChild(index), gameObjectInfos);
    }
}
