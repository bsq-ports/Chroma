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

MAKE_HOOK_OFFSETLESS(
    LightPairRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger,
    void,
    LightPairRotationEventEffect* self,
    CustomBeatmapEventData* beatmapEventData
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

MAKE_HOOK_OFFSETLESS(
    LightPairRotationEventEffect_UpdateRotationData,
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

    auto chromaData = std::static_pointer_cast<ChromaLaserSpeedEventData>(chromaIt->second);


    bool isLeftEvent = beatmapEventData->type == self->eventL;
    // rotationData
    LightPairRotationEventEffect::RotationData *customRotationData = isLeftEvent ? self->rotationDataL
                                                                                 : self->rotationDataR;


    bool lockPosition = chromaData->LockPosition;
    float precisionSpeed = chromaData->PreciseSpeed;
    int dir = chromaData->Direction;


    switch (dir) {
        case 0:
            direction = isLeftEvent ? -1.0f : 1.0f;
            break;

        case 1:
            direction = isLeftEvent ? 1.0f : -1.0f;
            break;
    }



    //getLogger().debug("The time is: %d", beatmapEventData->time);
    if (beatmapEventData->value == 0) {
        customRotationData->enabled = false;
        if (!lockPosition) {
            customRotationData->rotationAngle = customRotationData->startRotationAngle;
            customRotationData->transform->set_localRotation(
                    quaternionMultiply(customRotationData->startRotation,
                                       UnityEngine::Quaternion::Euler(
                                               vectorMultiply(self->rotationVector,
                                                              customRotationData->startRotationAngle)
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
                                       UnityEngine::Quaternion::Euler(
                                               vectorMultiply(self->rotationVector, rotationAngle)
                                       )
                    ));
        }
    }
}

void Chroma::Hooks::LightPairRotationEventEffect() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), LightPairRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger, il2cpp_utils::FindMethodUnsafe("", "LightPairRotationEventEffect", "HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger", 1));
    INSTALL_HOOK_OFFSETLESS(getLogger(), LightPairRotationEventEffect_UpdateRotationData, il2cpp_utils::FindMethodUnsafe("", "LightPairRotationEventEffect", "UpdateRotationData", 4));
}