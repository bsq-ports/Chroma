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
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::GetChromaLegacy() && !ChromaController::DoChromaHooks()) {
        LightSwitchEventEffect_Start(self);
        return;
    }

    auto* coro = custom_types::Helpers::CoroutineHelper::New(WaitThenStart(self, self->event));

    self->StartCoroutine(reinterpret_cast<IEnumerator*>(coro));

    LightSwitchEventEffect_Start(self);
}

MAKE_HOOK_OFFSETLESS(LightSwitchEventEffect_SetColor, void, LightSwitchEventEffect* self, UnityEngine::Color color) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::GetChromaLegacy() && !ChromaController::DoChromaHooks()) {
        LightSwitchEventEffect_SetColor(self, color);
        return;
    }

    if (LightSwitchEventEffectHolder::LightIDOverride){
        auto lights = LightSwitchEventEffectHolder::LightIDOverride.value();

        // TODO: https://github.com/Aeroluna/Chroma/commit/a8fc978b282af145c6ed263bfcce3485a31bb039#diff-cb9d6e4e838e56a6d827e7121a5846a8685381e0f7594e598a33e6b7f097cc98R36-R43

        for (auto & light : lights){
            if (!light) continue;
            light->ColorWasSet(color);
        }
        // TODO: Uncomment when LightID rework
        //        LightSwitchEventEffectHolder::LightIDOverride = std::nullopt;



        return;
    } else {
        LightSwitchEventEffect_SetColor(self, color);
    }
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

    // TODO: Remove
    LightSwitchEventEffectHolder::LightIDOverride = std::nullopt;
}

void Chroma::Hooks::LightSwitchEventEffect() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), LightSwitchEventEffect_Start, il2cpp_utils::FindMethodUnsafe("", "LightSwitchEventEffect", "Start", 0));
    INSTALL_HOOK_OFFSETLESS(getLogger(), LightSwitchEventEffect_SetColor, il2cpp_utils::FindMethodUnsafe("", "LightSwitchEventEffect", "SetColor", 1));
    INSTALL_HOOK_OFFSETLESS(getLogger(), LightSwitchEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger, il2cpp_utils::FindMethodUnsafe("", "LightSwitchEventEffect", "HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger", 1));
}