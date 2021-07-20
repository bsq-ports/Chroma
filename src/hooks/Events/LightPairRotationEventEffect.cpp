#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "utils/ChromaUtils.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/BeatmapEventData.hpp"
#include "GlobalNamespace/BeatmapEventType.hpp"
#include "GlobalNamespace/LightPairRotationEventEffect.hpp"
#include "GlobalNamespace/LightPairRotationEventEffect_RotationData.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Transform.hpp"
#include "utils/ChromaUtils.hpp"
#include "lighting/ChromaEventData.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Chroma;
using namespace ChromaUtils;

static BeatmapEventData* LastLightPairRotationEventEffectData;

MAKE_HOOK_MATCH(
    LightPairRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger,
    &LightPairRotationEventEffect::HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger,
    void,
    LightPairRotationEventEffect* self,
    BeatmapEventData* beatmapEventData
) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        LightPairRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
        return;
    }

    if (beatmapEventData->type == self->eventL || beatmapEventData->type == self->eventR) {
        LastLightPairRotationEventEffectData = beatmapEventData;
    }

//    getLogger().debug("Doing lights");
    LightPairRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
//    getLogger().debug("Did the custom lights");

    LastLightPairRotationEventEffectData = nullptr;
}

MAKE_HOOK_MATCH(
    LightPairRotationEventEffect_UpdateRotationData,
    &LightPairRotationEventEffect::UpdateRotationData,
    void,
    LightPairRotationEventEffect* self,
    int beatmapEventDataValue,
    LightPairRotationEventEffect::RotationData* rotationData,
    float startRotationOffset,
    float direction
) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        LightPairRotationEventEffect_UpdateRotationData(self, beatmapEventDataValue, rotationData, startRotationOffset,
                                                        direction);
        return;
    }


    auto beatmapEventData = LastLightPairRotationEventEffectData;

    auto chromaIt = ChromaEventDataManager::ChromaEventDatas.find(beatmapEventData);

    // Not found
    if (chromaIt == ChromaEventDataManager::ChromaEventDatas.end()) {
        LightPairRotationEventEffect_UpdateRotationData(self, beatmapEventDataValue, rotationData, startRotationOffset,
                                                        direction);
        return;
    }

    auto chromaData = chromaIt->second;


    bool isLeftEvent = beatmapEventData->type == self->eventL;
    // rotationData
    LightPairRotationEventEffect::RotationData *customRotationData = isLeftEvent ? self->rotationDataL
                                                                                 : self->rotationDataR;


    bool lockPosition = chromaData->LockPosition;
    float precisionSpeed = chromaData->Speed.value_or(beatmapEventData->value);
    std::optional<int> dir = chromaData->Direction;

    if (dir) {
        switch (*dir) {
            case 0:
                direction = isLeftEvent ? -1.0f : 1.0f;
                break;

            case 1:
                direction = isLeftEvent ? 1.0f : -1.0f;
                break;
        }
    }

    static auto* euler = &::il2cpp_utils::GetClassFromName("UnityEngine", "Vector3")->byval_arg;
    static const MethodInfo *QuaternionEuler = il2cpp_utils::FindMethod(classof(UnityEngine::Quaternion), "Euler", std::vector<Il2CppClass*>(), ::std::vector<const Il2CppType*>{euler});
    static auto QuaternionEulerMPtr = reinterpret_cast<UnityEngine::Quaternion(*)(UnityEngine::Vector3)>(QuaternionEuler->methodPointer);

    //getLogger().debug("The time is: %d", beatmapEventData->time);
    if (beatmapEventData->value == 0) {
        customRotationData->enabled = false;
        if (!lockPosition) {
            customRotationData->rotationAngle = customRotationData->startRotationAngle;
            customRotationData->transform->set_localRotation(
                    quaternionMultiply(customRotationData->startRotation,
                                       QuaternionEulerMPtr(
                                               vectorMultiply(self->rotationVector, customRotationData->startRotationAngle)
                                       ))
            );
        }
    } else if (beatmapEventData->value > 0) {
        customRotationData->enabled = true;
        customRotationData->rotationSpeed = precisionSpeed * 20.0f * direction;
        //getLogger().debug("Doing rotation speed (%d) %d", beatmapEventData->value, customRotationData->rotationSpeed);
        if (!lockPosition) {
            float rotationAngle = startRotationOffset + customRotationData->startRotationAngle;
            customRotationData->rotationAngle = rotationAngle;
            customRotationData->transform->set_localRotation(
                    quaternionMultiply(customRotationData->startRotation,
                                       QuaternionEulerMPtr(
                                               vectorMultiply(self->rotationVector, rotationAngle)
                                       )
                    ));
        }
    }
}

void LightPairRotationEventEffectHook(Logger& logger) {
    INSTALL_HOOK(getLogger(), LightPairRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger);
    INSTALL_HOOK(getLogger(), LightPairRotationEventEffect_UpdateRotationData);
}

ChromaInstallHooks(LightPairRotationEventEffectHook)