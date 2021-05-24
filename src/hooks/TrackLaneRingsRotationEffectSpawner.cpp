#include "main.hpp"
#include "Chroma.hpp"
#include "utils/ChromaUtils.hpp"
#include "GlobalNamespace/TrackLaneRingsRotationEffectSpawner.hpp"
#include "GlobalNamespace/TrackLaneRingsRotationEffect.hpp"
#include "lighting/ChromaRingsRotationEffect.hpp"
#include "UnityEngine/Random.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Object.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"
#include <cstring>
#include "ChromaController.hpp"
#include "lighting/ChromaEventData.hpp"

using namespace Chroma;
using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace ChromaUtils;



MAKE_HOOK_OFFSETLESS(TrackLaneRingsRotationEffectSpawner_Start, void, GlobalNamespace::TrackLaneRingsRotationEffectSpawner* self) {
    static auto contextLogger = getLogger().WithContext(Chroma::ChromaLogger::TrackLaneRings);
    if (!ChromaController::DoChromaHooks()) {
        TrackLaneRingsRotationEffectSpawner_Start(self);
        return;
    }

    static auto TrackLaneRingsRotationEffectKlass = classof(TrackLaneRingsRotationEffect*);

    if (self->trackLaneRingsRotationEffect->klass == TrackLaneRingsRotationEffectKlass) {
        auto *oldRotationEffect = self->trackLaneRingsRotationEffect;
        debugSpamLog(contextLogger, "Adding component");
        auto newRotationEffect = oldRotationEffect->get_gameObject()->AddComponent<ChromaRingsRotationEffect *>();
        debugSpamLog(contextLogger, "Copyying values now");
        newRotationEffect->CopyValues(oldRotationEffect);

        UnityEngine::Object::Destroy(oldRotationEffect);

        self->trackLaneRingsRotationEffect = il2cpp_utils::cast<TrackLaneRingsRotationEffect>(newRotationEffect);
    }
    TrackLaneRingsRotationEffectSpawner_Start(self);
}


template<typename T>
T getValueOrDefault(rapidjson::Value* val, const std::string& s, T def) {
    auto v = val->FindMember(s);
    return v != val->MemberEnd() ? v->value.Get<T>() : def;
}

void TriggerRotation(
        TrackLaneRingsRotationEffect* trackLaneRingsRotationEffect,
        bool rotRight,
        float rotation,
        float rotationStep,
        float rotationPropagationSpeed,
        float rotationFlexySpeed)
{
    static auto contextLogger = getLogger().WithContext(Chroma::ChromaLogger::TrackLaneRings);
    debugSpamLog(contextLogger, "DOING TRIGGER ROTATION %s", trackLaneRingsRotationEffect->klass->name);

    auto chromaRingRotation = il2cpp_utils::cast<ChromaRingsRotationEffect>(trackLaneRingsRotationEffect);

    chromaRingRotation->AddRingRotationEffectF(chromaRingRotation->GetFirstRingDestinationRotationAngle() + (rotation * (rotRight ? -1.0f : 1.0f)), rotationStep, rotationPropagationSpeed, rotationFlexySpeed);
}



// TODO:
// This method is directly ported from TrackLaneRingsRotationEffectSpawner. It is required to be ported since for some inexplicable reason
// using the original method causes CJD or something else to stop loading the map and it
// just stays as limbo. Hopefully with time we can fix that and use that instead
void origHandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(GlobalNamespace::TrackLaneRingsRotationEffectSpawner* self, BeatmapEventData* beatmapEventData) {
    static auto contextLogger = getLogger().WithContext(Chroma::ChromaLogger::TrackLaneRings);
    if (beatmapEventData->type != self->beatmapEventType) {
        return;
    }
    float step = 0.0f;

    int originalRotationStepType = (int) self->rotationStepType;

    if (originalRotationStepType == TrackLaneRingsRotationEffectSpawner::RotationStepType::Range0ToMax) {
        step = UnityEngine::Random::Range(0.0f, self->rotationStep);
    } else if (originalRotationStepType == TrackLaneRingsRotationEffectSpawner::RotationStepType::Range) {
        step = UnityEngine::Random::Range(-self->rotationStep, self->rotationStep);
    } else if (originalRotationStepType == TrackLaneRingsRotationEffectSpawner::RotationStepType::MaxOr0) {
        step = (UnityEngine::Random::get_value() < 0.5f) ? self->rotationStep : 0.0f;
    }
    debugSpamLog(contextLogger, "Track lane klass %s", self->trackLaneRingsRotationEffect->klass->name);

    static auto ChromaRingsRotationEffectKlass = classof(ChromaRingsRotationEffect*);

    auto rotationEffect = self->trackLaneRingsRotationEffect;


    if (ASSIGNMENT_CHECK(ChromaRingsRotationEffectKlass, self->trackLaneRingsRotationEffect->klass)) {

        reinterpret_cast<ChromaRingsRotationEffect*>
        (rotationEffect)->AddRingRotationEffectF(
                self->trackLaneRingsRotationEffect->GetFirstRingRotationAngle() + self->rotation * (float) ((UnityEngine::Random::get_value() < 0.5f) ? 1 : -1),
                step,
                (float) self->rotationPropagationSpeed,
                self->rotationFlexySpeed);
    } else {
        rotationEffect->AddRingRotationEffect(
                self->trackLaneRingsRotationEffect->GetFirstRingRotationAngle() + self->rotation * (float) ((UnityEngine::Random::get_value() < 0.5f) ? 1 : -1),
                step,
                self->rotationPropagationSpeed,
                self->rotationFlexySpeed);
    }
}

MAKE_HOOK_OFFSETLESS(TrackLaneRingsRotationEffectSpawner_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger, void, GlobalNamespace::TrackLaneRingsRotationEffectSpawner* self,
                     BeatmapEventData* beatmapEventData) {
    if (!ChromaController::DoChromaHooks()) {
        TrackLaneRingsRotationEffectSpawner_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self,
                                                                                                        beatmapEventData);
        return;
    }


//    debugSpamLog(contextLogger, "Track lane rotation effect self %d beat %d and customData %d", self->beatmapEventType.value,
//                      beatmapEventData->type.value,
//                      beatmapEventData->customData != nullptr && beatmapEventData->customData->value != nullptr ? 0 : 1);
    static auto contextLogger = getLogger().WithContext(Chroma::ChromaLogger::TrackLaneRings);

    if (beatmapEventData->type == beatmapEventData->type) {
        auto chromaIt = ChromaEventDataManager::ChromaEventDatas.find(beatmapEventData);

        // Not found
        if (chromaIt == ChromaEventDataManager::ChromaEventDatas.end()) {
            origHandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
            return;
        }

        auto chromaData = std::static_pointer_cast<ChromaRingRotationEventData>(chromaIt->second);


        debugSpamLog(contextLogger, "Doing stuff with custom Data ring %s", chromaData->dataType == EventDataType::RING ? "true" : "false");

        if (chromaData->dataType == EventDataType::RING) {

            float rotationStep = 0.0f;

            float originalRotationStep = self->rotationStep;
            float originalRotation = self->rotation;
            auto originalRotationPropagationSpeed = (float) self->rotationPropagationSpeed;
            float originalRotationFlexySpeed = self->rotationFlexySpeed;
            int originalRotationStepType = (int) self->rotationStepType;

            if (originalRotationStepType == TrackLaneRingsRotationEffectSpawner::RotationStepType::Range0ToMax) {
                rotationStep = UnityEngine::Random::Range(0.0f, rotationStep);
            } else if (originalRotationStepType == TrackLaneRingsRotationEffectSpawner::RotationStepType::Range) {
                rotationStep = UnityEngine::Random::Range(-originalRotationStep, originalRotationStep);
            } else if (originalRotationStepType == TrackLaneRingsRotationEffectSpawner::RotationStepType::MaxOr0) {
                rotationStep = (UnityEngine::Random::get_value() < 0.5f) ? originalRotationStep : 0.0f;
            }

            debugSpamLog(contextLogger, "Got the data");
            auto selfName = to_utf8(csstrtostr(self->get_name()));

            auto nameFilter = chromaData->NameFilter;
            if (nameFilter && stringCompare(selfName, nameFilter.value()) != 0) {
                debugSpamLog(contextLogger, "Name filter ignored");
                return;
            }

            auto dir = chromaData->Direction;

            // https://github.com/Aeroluna/Chroma/commit/3900969d3fef1eaeea745bcfc23c61bfbe525586#diff-e83fa5da7e2e725f2cfb2ee5a6d6a085b2065a95e0d4757e01fe3c29f0fa4024
            bool rotRight;
            if (dir) {
                rotRight = UnityEngine::Random::get_value() < 0.5f;
            } else {
                rotRight = dir == 1;
            }

            auto counterSpinV = chromaData->CounterSpin;
            if (counterSpinV && counterSpinV.value()) {
                if (selfName.find("Big") == std::string::npos) {
                    rotRight = !rotRight;
                }
            }

            auto reset = chromaData->Reset;
            if (reset && reset.value()) {
                debugSpamLog(contextLogger, "Reset spawn, returning");
                TriggerRotation(self->trackLaneRingsRotationEffect, rotRight, originalRotation, 0, 50, 50);
                return;
            }

            debugSpamLog(contextLogger, "Getting the last values");

            float step = chromaData->Step.value_or(rotationStep);
            float prop = chromaData->Prop.value_or(originalRotationPropagationSpeed);
            float speed = chromaData->Speed.value_or(originalRotationFlexySpeed);
            float rotation = chromaData->Rotation.value_or(originalRotation);

            float stepMult = chromaData->SpeedMult;
            float propMult = chromaData->PropMult;
            float speedMult = chromaData->SpeedMult;

            TriggerRotation(self->trackLaneRingsRotationEffect, rotRight, rotation, step * stepMult, prop * propMult,
                            speed * speedMult);
            debugSpamLog(contextLogger, "Finished spawn, returning");
            return;
        }
    }


    debugSpamLog(contextLogger, "Not a custom beat map");
    origHandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);//        TrackLaneRingsRotationEffectSpawner_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
}




void Hooks::TrackLaneRingsRotationEffectSpawner() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), TrackLaneRingsRotationEffectSpawner_Start, il2cpp_utils::FindMethodUnsafe("", "TrackLaneRingsRotationEffectSpawner", "Start", 0));
    INSTALL_HOOK_OFFSETLESS(getLogger(), TrackLaneRingsRotationEffectSpawner_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger, il2cpp_utils::FindMethodUnsafe("", "TrackLaneRingsRotationEffectSpawner", "HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger", 1));
    //    INSTALL_HOOK_OFFSETLESS(getLogger(), SaberManager_Finalize, il2cpp_utils::FindMethodUnsafe("System", "Object", "Finalize", 0));
}