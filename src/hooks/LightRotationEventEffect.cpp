#include "Chroma.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/LightRotationEventEffect.hpp"
#include "UnityEngine/Random.hpp"
#include "UnityEngine/Space.hpp"
#include "UnityEngine/Transform.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Chroma;

MAKE_HOOK_OFFSETLESS(
    LightRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger,
    void,
    LightRotationEventEffect* self,
    CustomBeatmapEventData* beatmapEventData
) {
    if (beatmapEventData->type == self->event) {
        bool isLeftEvent = self->event == BeatmapEventType::Event12;

        bool isCustomData = beatmapEventData->customData && beatmapEventData->customData->value;
        rapidjson::Value* dynData = isCustomData ? beatmapEventData->customData->value : nullptr;

        bool lockPosition = isCustomData && dynData->HasMember(LOCKPOSITION) && dynData->FindMember(LOCKPOSITION)->value.GetBool();

        float precisionSpeed = isCustomData && dynData->HasMember(PRECISESPEED) ? dynData->FindMember(PRECISESPEED)->value.GetFloat() : (float) beatmapEventData->value;

        int dir = isCustomData && dynData->HasMember(DIRECTION) ? dynData->FindMember(DIRECTION)->value.GetInt() : -1;

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