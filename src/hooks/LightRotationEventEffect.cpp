#include "Chroma.hpp"
#include "ChromaController.hpp"
#include "utils/ChromaUtils.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/LightRotationEventEffect.hpp"
#include "UnityEngine/Random.hpp"
#include "UnityEngine/Space.hpp"
#include "UnityEngine/Transform.hpp"

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


    if (beatmapEventData->type == self->event && il2cpp_functions::class_is_assignable_from(classof(CustomBeatmapEventData*), beatmapEventData->klass)) {
        auto* customBeatmapEvent = reinterpret_cast<CustomBeatmapEventData *>(beatmapEventData);
        bool isLeftEvent = self->event == BeatmapEventType::Event12;

        bool isCustomData = customBeatmapEvent->customData && customBeatmapEvent->customData->value && customBeatmapEvent->customData->value->IsObject();
        rapidjson::Value* dynData = isCustomData ? customBeatmapEvent->customData->value : nullptr;

        bool lockPosition = getIfExists(dynData, LOCKPOSITION, false);

        float precisionSpeed = getIfExists(dynData, PRECISESPEED, (float) beatmapEventData->value);

        int dir = getIfExists(dynData, DIRECTION, -1);

        float direction = (Random::get_value() > 0.5f) ? 1.0f : -1.0f;
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
                self->get_transform()->Rotate(self->rotationVector, Random::Range(0.0f, 180.0f), Space::Self);
            }
        }
    } else {
        LightRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
    }
}

void Chroma::Hooks::LightRotationEventEffect() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), LightRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger, il2cpp_utils::FindMethodUnsafe("", "LightRotationEventEffect", "HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger", 1));
}