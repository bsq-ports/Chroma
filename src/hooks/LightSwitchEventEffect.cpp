#include "Chroma.hpp"
#include "ChromaController.hpp"

#include <vector>
#include <optional>
#include "lighting/LightColorManager.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"
#include "System/Collections/Generic/IEnumerator_1.hpp"
#include "colorizer/LightColorizer.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/register.hpp"
#include "System/Collections/IEnumerator.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/WaitForEndOfFrame.hpp"
#include "GlobalNamespace/ILightWithId.hpp"
#include "hooks/LightSwitchEventEffect.hpp"
#include "lighting/LightIDTableManager.hpp"

#include <experimental/coroutine>

using namespace CustomJSONData;
using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;
using namespace custom_types::Helpers;



custom_types::Helpers::Coroutine WaitThenStart(LightSwitchEventEffect *instance, BeatmapEventType eventType) {
    CRASH_UNLESS(instance);
    co_yield reinterpret_cast<IEnumerator*>(CRASH_UNLESS(WaitForEndOfFrame::New_ctor()));
    LightColorizer::LSEStart(instance, eventType);
    co_return;
}

MAKE_HOOK_OFFSETLESS(LightSwitchEventEffect_Start, void, LightSwitchEventEffect* self) {
    LightSwitchEventEffect_Start(self);

    // Do nothing if Chroma shouldn't run
    if (!ChromaController::GetChromaLegacy() && !ChromaController::DoChromaHooks()) {
        return;
    }

    auto* coro = custom_types::Helpers::CoroutineHelper::New(WaitThenStart(self, self->event));

    self->StartCoroutine(reinterpret_cast<IEnumerator*>(coro));

}

MAKE_HOOK_OFFSETLESS(LightSwitchEventEffect_SetColor, void, LightSwitchEventEffect* self, UnityEngine::Color color) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::GetChromaLegacy() && !ChromaController::DoChromaHooks()) {
        LightSwitchEventEffect_SetColor(self, color);
        return;
    }


    if (LightSwitchEventEffectHolder::LightIDOverride) {
        auto lights = LightColorizer::GetLights(self);

        int type = self->event;
        std::vector<int> newIds;

        for (auto id : LightSwitchEventEffectHolder::LightIDOverride.value()) {
            auto newId = LightIDTableManager::GetActiveTableValue(type, id);

            if (newId) {
                newIds.push_back(newId.value());
            } else {
                newIds.push_back(id);
            }
        }


        for (auto id : newIds) {
            if (id < 0 || id > lights.size()) {
                getLogger().warning("Type %d does not contain id %d", type, id);
            } else {
                auto l = lights[id];
                if (l) {
                    if (l->get_isRegistered()) {
                        l->ColorWasSet(color);
                    }
                }
            }
        }

        LightSwitchEventEffectHolder::LightIDOverride = std::nullopt;

        return;
    }

    if (LightSwitchEventEffectHolder::LegacyLightOverride) {
        auto lights = LightSwitchEventEffectHolder::LegacyLightOverride.value();

        for (auto &light : lights) {
            if (!light) continue;
            light->ColorWasSet(color);
        }

        LightSwitchEventEffectHolder::LegacyLightOverride = std::nullopt;

        return;
    }

    LightSwitchEventEffect_SetColor(self, color);

}

MAKE_HOOK_OFFSETLESS(LightSwitchEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger, void, LightSwitchEventEffect* self, CustomBeatmapEventData* beatmapEventData) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::GetChromaLegacy() && !ChromaController::DoChromaHooks()) {
        LightSwitchEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
        return;
    }



    if (beatmapEventData->type == self->event) {
        LightColorizer::SetLastValue(self, beatmapEventData->value);
        LightColorManager::ColorLightSwitch(self, beatmapEventData);
    }

    LightSwitchEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
}

void Chroma::Hooks::LightSwitchEventEffect() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), LightSwitchEventEffect_Start, il2cpp_utils::FindMethodUnsafe("", "LightSwitchEventEffect", "Start", 0));
    INSTALL_HOOK_OFFSETLESS(getLogger(), LightSwitchEventEffect_SetColor, il2cpp_utils::FindMethodUnsafe("", "LightSwitchEventEffect", "SetColor", 1));
    INSTALL_HOOK_OFFSETLESS(getLogger(), LightSwitchEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger, il2cpp_utils::FindMethodUnsafe("", "LightSwitchEventEffect", "HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger", 1));
}