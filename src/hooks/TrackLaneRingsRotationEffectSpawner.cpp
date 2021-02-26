#include "main.hpp"
#include "Chroma.hpp"
#include "utils/ChromaUtils.hpp"
#include "GlobalNamespace/TrackLaneRingsRotationEffectSpawner.hpp"
#include "GlobalNamespace/TrackLaneRingsRotationEffect.hpp"
#include "ChromaRingsRotationEffect.hpp"
#include "UnityEngine/Random.hpp"
#include "UnityEngine/GameObject.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"
#include <cstring>

using namespace Chroma;
using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;

MAKE_HOOK_OFFSETLESS(TrackLaneRingsRotationEffectSpawner_Start, void, GlobalNamespace::TrackLaneRingsRotationEffectSpawner* self) {
    auto* oldRotationEffect = self->trackLaneRingsRotationEffect;
    getLogger().debug("Adding component");
    auto newRotationEffect = oldRotationEffect->get_gameObject()->AddComponent<ChromaRingsRotationEffect*>();
    getLogger().debug("Copyying values now");
    newRotationEffect->CopyValues(oldRotationEffect);

    self->trackLaneRingsRotationEffect = newRotationEffect;
    TrackLaneRingsRotationEffectSpawner_Start(self);
}

// Should replace with an actual standard solution
int charDiff(char c1, char c2)
{
    if ( tolower(c1) < tolower(c2) ) return -1;
    if ( tolower(c1) == tolower(c2) ) return 0;
    return 1;
}

int stringCompare(const std::string& str1, const std::string& str2)
{
    int diff = 0;
    int size = std::min(str1.size(), str2.size());
    for (size_t idx = 0; idx < size && diff == 0; ++idx)
    {
        diff += charDiff(str1[idx], str2[idx]);
    }
    if ( diff != 0 ) return diff;

    if ( str2.length() == str1.length() ) return 0;
    if ( str2.length() > str1.length() ) return 1;
    return -1;
}

template<typename T>
T getValueOrDefault(rapidjson::Value*& val, const std::string& s, T def) {
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
    getLogger().debug("DOING TRIGGER ROTATION");
    reinterpret_cast<ChromaRingsRotationEffect*>(trackLaneRingsRotationEffect)->AddRingRotationEffectF(trackLaneRingsRotationEffect->GetFirstRingDestinationRotationAngle() + (rotation * (rotRight ? -1.0f : 1.0f)), rotationStep, rotationPropagationSpeed, rotationFlexySpeed);
}

MAKE_HOOK_OFFSETLESS(TrackLaneRingsRotationEffectSpawner_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger, void, GlobalNamespace::TrackLaneRingsRotationEffectSpawner* self, CustomJSONData::CustomBeatmapEventData* beatmapEventData) {

    getLogger().debug("Track lane rotation effect self %d beat %d and customData %d", self->beatmapEventType.value, beatmapEventData->type.value, beatmapEventData->customData ==
            nullptr ? 0 : 1);
//    if (il2cpp_functions::class_is_assignable_from(beatmapEventData->klass, classof(CustomJSONData::CustomBeatmapEventData*))) {
        auto *customBeatmapEvent = reinterpret_cast<CustomBeatmapEventData *>(beatmapEventData);

        if (self->beatmapEventType.value == beatmapEventData->type.value && customBeatmapEvent->customData) {
            getLogger().debug("Doing stuff with custom Data");
            float rotationStep = 0.0f;
            float originalRotationStep = self->rotationStep;
            float originalRotation = self->rotation;
            float originalRotationPropagationSpeed = self->rotationPropagationSpeed;
            float originalRotationFlexySpeed = self->rotationFlexySpeed;
            int originalRotationStepType = (int) self->rotationStepType;

            if (originalRotationStepType == TrackLaneRingsRotationEffectSpawner::RotationStepType::Range0ToMax) {
                rotationStep = UnityEngine::Random::Range(0.0f, rotationStep);
            } else if (originalRotationStepType == TrackLaneRingsRotationEffectSpawner::RotationStepType::Range) {
                rotationStep = UnityEngine::Random::Range(-originalRotationStep, originalRotationStep);
            } else if (originalRotationStepType == TrackLaneRingsRotationEffectSpawner::RotationStepType::MaxOr0) {
                rotationStep = (UnityEngine::Random::get_value() < 0.5f) ? originalRotationStep : 0.0f;
            }

            auto dynData = customBeatmapEvent->customData->value;
            getLogger().debug("Got the data");
            auto selfName = to_utf8(csstrtostr(self->get_name()));

            auto nameFilter = dynData->FindMember(NAMEFILTER);
            if (nameFilter != dynData->MemberEnd() && stringCompare(selfName, nameFilter->value.GetString()) == 0) {
                getLogger().debug("Name filter ignored");
                return;
            }

            int dir;

            auto dirV = dynData->FindMember(DIRECTION);

            if (dirV == dynData->MemberEnd()) {
                dir = -1;
            } else dir = dirV->value.GetInt();

            bool rotRight;
            if (dir == -1) {
                rotRight = UnityEngine::Random::get_value() < 0.5f;
            } else {
                rotRight = dir == 1;
            }

            auto counterSpinV = dynData->FindMember(COUNTERSPIN);
            if (counterSpinV != dynData->MemberEnd() && counterSpinV->value.GetBool()) {
                if (selfName.find("Big") == std::string::npos) {
                    rotRight = !rotRight;
                }
            }

            auto reset = dynData->FindMember(RESET);
            if (reset != dynData->MemberEnd() && reset->value.GetBool()) {
                getLogger().debug("Reset spawn, returning");
                TriggerRotation(self->trackLaneRingsRotationEffect, rotRight, originalRotation, 0, 50, 50);
                return;
            }

            getLogger().debug("Getting the last values");

            float step = getValueOrDefault(dynData, STEP, rotationStep);
            float prop = getValueOrDefault(dynData, PROP, originalRotationStep);
            float speed = getValueOrDefault(dynData, SPEED, originalRotationFlexySpeed);
            float rotation = getValueOrDefault(dynData, ROTATION, originalRotation);

            float stepMult = getValueOrDefault(dynData, STEPMULT, 1.0f);
            float propMult = getValueOrDefault(dynData, PROPMULT, 1.0f);
            float speedMult = getValueOrDefault(dynData, SPEEDMULT, 1.0f);

            TriggerRotation(self->trackLaneRingsRotationEffect, rotRight, rotation, step * stepMult, prop * propMult,
                            speed * speedMult);
            getLogger().debug("Finished spawn, returning");
            return;
        }
//    }
    getLogger().debug("Not a custom beat map");
    TrackLaneRingsRotationEffectSpawner_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
}



void Hooks::TrackLaneRingsRotationEffectSpawner() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), TrackLaneRingsRotationEffectSpawner_Start, il2cpp_utils::FindMethodUnsafe("", "TrackLaneRingsRotationEffectSpawner", "Start", 0));
    INSTALL_HOOK_OFFSETLESS(getLogger(), TrackLaneRingsRotationEffectSpawner_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger, il2cpp_utils::FindMethodUnsafe("", "TrackLaneRingsRotationEffectSpawner", "HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger", 1));
    //    INSTALL_HOOK_OFFSETLESS(getLogger(), SaberManager_Finalize, il2cpp_utils::FindMethodUnsafe("System", "Object", "Finalize", 0));
}