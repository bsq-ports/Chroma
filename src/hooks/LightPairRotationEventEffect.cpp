
#include "Chroma.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/BeatmapEventData.hpp"
#include "GlobalNamespace/BeatmapEventType.hpp"
#include "GlobalNamespace/LightPairRotationEventEffect.hpp"
#include "GlobalNamespace/LightPairRotationEventEffect_RotationData.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Transform.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;

static CustomBeatmapEventData* LastLightPairRotationEventEffectData;

MAKE_HOOK_OFFSETLESS(
    LightPairRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger,
    void,
    LightPairRotationEventEffect* self,
    CustomBeatmapEventData* beatmapEventData
) {
    if (beatmapEventData->type == self->eventL || beatmapEventData->type == self->eventR) {
        LastLightPairRotationEventEffectData = beatmapEventData;
    }

    LightPairRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);

    LastLightPairRotationEventEffectData = nullptr;
}

MAKE_HOOK_OFFSETLESS(
    LightPairRotationEventEffect_UpdateRotationData,
    void,
    LightPairRotationEventEffect* self,
    int beatmapEventDataValue,
    LightPairRotationEventEffect::RotationData* rotationData,
    float startRotationOffset,
    float direction
) {
    CustomBeatmapEventData* beatmapEventData = LastLightPairRotationEventEffectData;

    bool isLeftEvent = beatmapEventData->type == self->eventL;

    // rotationData
    LightPairRotationEventEffect::RotationData* customRotationData = isLeftEvent ? self->rotationDataL : self->rotationDataR;

    if (beatmapEventData->customData) {
        rapidjson::Value &dynData = *beatmapEventData->customData;

        bool lockPosition = dynData.HasMember("_lockPosition") ? dynData["_lockPosition"].GetBool() : false;

        float precisionSpeed = dynData.HasMember("_preciseSpeed") ? dynData["_preciseSpeed"].GetFloat() : beatmapEventData->value;

        int dir = dynData.HasMember("_direction") ? dynData["_direction"].GetInt() : -1;

        switch (dir) {
        case 0:
            direction = isLeftEvent ? -1.0f : 1.0f;
            break;
                
        case 1:
            direction = isLeftEvent ? 1.0f : -1.0f;
            break;
        }

        if (beatmapEventData->value == 0) {
            customRotationData->enabled = false;
            if (!lockPosition) {
                customRotationData->rotationAngle = customRotationData->startRotationAngle;
                customRotationData->transform->set_localRotation(customRotationData->startRotation * UnityEngine::Quaternion::Euler(self->rotationVector * customRotationData->startRotationAngle));
            }
        } else if (beatmapEventData->value > 0) {
            customRotationData->enabled = true;
            customRotationData->rotationSpeed = precisionSpeed * 20.0f * direction;
            if (!lockPosition) {
                float rotationAngle = startRotationOffset + customRotationData->startRotationAngle;
                customRotationData->rotationAngle = rotationAngle;
                customRotationData->transform->set_localRotation(customRotationData->startRotation * UnityEngine::Quaternion::Euler(self->rotationVector * rotationAngle));
            }
        }
    } else {
        LightPairRotationEventEffect_UpdateRotationData(self, beatmapEventDataValue, rotationData, startRotationOffset, direction);
    }
}

void Chroma::Hooks::LightPairRotationEventEffect() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), LightPairRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger, il2cpp_utils::FindMethodUnsafe("", "LightPairRotationEventEffect", "HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger", 1));
    INSTALL_HOOK_OFFSETLESS(getLogger(), LightPairRotationEventEffect_UpdateRotationData, il2cpp_utils::FindMethodUnsafe("", "LightPairRotationEventEffect", "UpdateRotationData", 4));
}