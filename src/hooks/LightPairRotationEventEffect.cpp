
#include "Chroma.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/BeatmapEventData.hpp"
#include "GlobalNamespace/BeatmapEventType.hpp"
#include "GlobalNamespace/LightPairRotationEventEffect.hpp"
#include "GlobalNamespace/LightPairRotationEventEffect_RotationData.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Transform.hpp"
#include "utils/ChromaUtils.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Chroma;
using namespace ChromaUtilities;

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

//    getLogger().debug("Doing lights");
    LightPairRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
//    getLogger().debug("Did the custom lights");

    LastLightPairRotationEventEffectData = nullptr;
}

UnityEngine::Vector3 vectorMultiply(UnityEngine::Vector3 vector, float m) {
    return UnityEngine::Vector3(vector.x * m, vector.y * m, vector.z * m);
}

UnityEngine::Quaternion quaternionMultiply(UnityEngine::Quaternion lhs, UnityEngine::Quaternion rhs)
{
    return UnityEngine::Quaternion(lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y, lhs.w * rhs.y + lhs.y * rhs.w + lhs.z * rhs.x - lhs.x * rhs.z, lhs.w * rhs.z + lhs.z * rhs.w + lhs.x * rhs.y - lhs.y * rhs.x, lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z);
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
    CustomBeatmapEventData *beatmapEventData = LastLightPairRotationEventEffectData;

    bool isLeftEvent = beatmapEventData->type == self->eventL;

    // rotationData
    LightPairRotationEventEffect::RotationData *customRotationData = isLeftEvent ? self->rotationDataL
                                                                                 : self->rotationDataR;

//    if (beatmapEventData->customData && beatmapEventData->customData->value) {

    bool isCustomData = beatmapEventData->customData && beatmapEventData->customData->value;
    rapidjson::Value *dynData = isCustomData ? beatmapEventData->customData->value : nullptr;



//        bool lockPosition = dynData.HasMember(LOCKPOSITION) && dynData[LOCKPOSITION].GetBool();
    bool lockPosition =
            isCustomData && dynData->HasMember(LOCKPOSITION) && dynData->FindMember(LOCKPOSITION)->value.GetBool();

    float precisionSpeed =
            isCustomData && dynData->HasMember(PRECISESPEED) ? dynData->FindMember(PRECISESPEED)->value.GetFloat()
                                                             : (float) beatmapEventData->value;

    int dir = isCustomData && dynData->HasMember(DIRECTION) ? dynData->FindMember(DIRECTION)->value.GetInt() : -1;

    switch (dir) {
        case 0:
            direction = isLeftEvent ? -1.0f : 1.0f;
            break;

        case 1:
            direction = isLeftEvent ? 1.0f : -1.0f;
            break;
    }


    getLogger().debug("The time is: %d", beatmapEventData->time);
    if (beatmapEventData->value == 0) {
        customRotationData->enabled = false;
        if (!lockPosition) {
            customRotationData->rotationAngle = customRotationData->startRotationAngle;
            customRotationData->transform->set_localRotation(quaternionMultiply(customRotationData->startRotation,
                                                                                UnityEngine::Quaternion::Euler(
                                                                                        vectorMultiply(
                                                                                                self->rotationVector,
                                                                                                customRotationData->startRotationAngle))));
//                getLogger().debug("Doing rotation %d and local rot %d %d %d", customRotationData->rotationAngle, customRotationData->transform->get_localRotation().x,customRotationData->transform->get_localRotation().y,customRotationData->transform->get_localRotation().z);
            getLogger().debug("Pair | (beatmapEventData.value > 0) time = %d, localRotation = %s",
                              beatmapEventData->time,
                              quaternionStr(self->get_transform()->get_localRotation()).c_str());
        }
    } else if (beatmapEventData->value > 0) {
        customRotationData->enabled = true;
        customRotationData->rotationSpeed = precisionSpeed * 20.0f * direction;
        getLogger().debug("Doing rotation speed (%d) %d", beatmapEventData->value, customRotationData->rotationSpeed);
        if (!lockPosition) {
            float rotationAngle = startRotationOffset + customRotationData->startRotationAngle;
            customRotationData->rotationAngle = rotationAngle;
            customRotationData->transform->set_localRotation(quaternionMultiply(customRotationData->startRotation,
                                                                                UnityEngine::Quaternion::Euler(
                                                                                        vectorMultiply(
                                                                                                self->rotationVector,
                                                                                                rotationAngle))));
//                getLogger().debug("Doing rotation %d and local rot %d %d %d", customRotationData->rotationAngle, customRotationData->transform->get_localRotation().x,customRotationData->transform->get_localRotation().y,customRotationData->transform->get_localRotation().z);
            getLogger().debug("Pair | (beatmapEventData.value > 0) time = %d, localRotation = %s",
                              beatmapEventData->time,
                              quaternionStr(self->get_transform()->get_localRotation()).c_str());
        }
    }
//    } else {
//        LightPairRotationEventEffect_UpdateRotationData(self, beatmapEventDataValue, rotationData, startRotationOffset, direction);
//    }
}

void Chroma::Hooks::LightPairRotationEventEffect() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), LightPairRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger, il2cpp_utils::FindMethodUnsafe("", "LightPairRotationEventEffect", "HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger", 1));
    INSTALL_HOOK_OFFSETLESS(getLogger(), LightPairRotationEventEffect_UpdateRotationData, il2cpp_utils::FindMethodUnsafe("", "LightPairRotationEventEffect", "UpdateRotationData", 4));
}