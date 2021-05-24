#include "Chroma.hpp"
#include "ChromaController.hpp"
#include "utils/ChromaUtils.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/LightRotationEventEffect.hpp"
#include "UnityEngine/Random.hpp"
#include "UnityEngine/Space.hpp"
#include "UnityEngine/Transform.hpp"

#include "lighting/ChromaEventData.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Chroma;
using namespace ChromaUtils;

MAKE_HOOK_OFFSETLESS(
    LightRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger,
    void,
    LightRotationEventEffect* self,
    BeatmapEventData* beatmapEventData
) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        LightRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
        return;
    }

    auto chromaIt = ChromaEventDataManager::ChromaEventDatas.find(beatmapEventData);

    // Not found
    if (chromaIt == ChromaEventDataManager::ChromaEventDatas.end()) {
        LightRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
        return;
    }

    auto chromaData = std::static_pointer_cast<ChromaLaserSpeedEventData>(chromaIt->second);


    bool isLeftEvent = self->event == BeatmapEventType::Event12;


    bool lockPosition = chromaData->LockPosition;

    float precisionSpeed = chromaData->PreciseSpeed;

    int dir = chromaData->Direction;

    float direction = (randomNumber() > 0.5f) ? 1.0f : -1.0f;
    switch (dir) {
        case 0:
            direction = isLeftEvent ? -1.0f : 1.0f;
            break;
        case 1:
            direction = isLeftEvent ? 1.0f : -1.0f;
            break;
    }

    if (beatmapEventData->value == 0) {
        self->set_enabled(false);
        if (!lockPosition) {
            self->get_transform()->set_localRotation(self->startRotation);
        }
    } else if (beatmapEventData->value > 0) {
        self->set_enabled(true);
        self->rotationSpeed = precisionSpeed * 20.0f * direction;
        if (!lockPosition) {
            self->get_transform()->set_localRotation(self->startRotation);
            self->get_transform()->Rotate(self->rotationVector, randomNumber(0.0f, 180.0f), Space::Self);
        }
    }
}

void Chroma::Hooks::LightRotationEventEffect() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), LightRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger, il2cpp_utils::FindMethodUnsafe("", "LightRotationEventEffect", "HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger", 1));
}