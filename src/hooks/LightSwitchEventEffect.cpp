#include "Chroma.hpp"
#include "ChromaController.hpp"

#include <vector>
#include <optional>
#include "LightColorManager.hpp"
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

std::optional<std::vector<GlobalNamespace::ILightWithId *>> LightSwitchEventEffectHolder::OverrideLightWithIdActivation = std::nullopt;

custom_types::Helpers::Coroutine WaitThenStart(LightSwitchEventEffect *instance, BeatmapEventType eventType) {
    CRASH_UNLESS(instance);
    co_yield reinterpret_cast<IEnumerator*>(CRASH_UNLESS(WaitForEndOfFrame::New_ctor()));
    LightColorizer::LSEStart(instance, eventType);
    co_return;
}

MAKE_HOOK_OFFSETLESS(LightSwitchEventEffect_Start, void, LightSwitchEventEffect* self) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        LightSwitchEventEffect_Start(self);
        return;
    }

    auto* coro = custom_types::Helpers::CoroutineHelper::New(WaitThenStart(self, self->event));

    self->StartCoroutine(reinterpret_cast<IEnumerator*>(coro));

    LightSwitchEventEffect_Start(self);
}

MAKE_HOOK_OFFSETLESS(LightSwitchEventEffect_SetColor, void, LightSwitchEventEffect* self, UnityEngine::Color color) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        LightSwitchEventEffect_SetColor(self, color);
        return;
    }

    if (LightSwitchEventEffectHolder::OverrideLightWithIdActivation){
        auto lights = LightSwitchEventEffectHolder::OverrideLightWithIdActivation.value();
        for (auto & light : lights){
            if (!light) continue;
            light->ColorWasSet(color);
        }
        return;
    } else {
        LightSwitchEventEffect_SetColor(self, color);
    }
}

MAKE_HOOK_OFFSETLESS(LightSwitchEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger, void, LightSwitchEventEffect* self, CustomBeatmapEventData* beatmapEventData) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        LightSwitchEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
        return;
    }

    if (beatmapEventData->type == self->event) {
        LightColorManager::ColorLightSwitch(self, beatmapEventData);
    }

    LightSwitchEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);

    LightSwitchEventEffectHolder::OverrideLightWithIdActivation = std::nullopt;
}

void Chroma::Hooks::LightSwitchEventEffect() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), LightSwitchEventEffect_Start, il2cpp_utils::FindMethodUnsafe("", "LightSwitchEventEffect", "Start", 0));
    INSTALL_HOOK_OFFSETLESS(getLogger(), LightSwitchEventEffect_SetColor, il2cpp_utils::FindMethodUnsafe("", "LightSwitchEventEffect", "SetColor", 1));
    INSTALL_HOOK_OFFSETLESS(getLogger(), LightSwitchEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger, il2cpp_utils::FindMethodUnsafe("", "LightSwitchEventEffect", "HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger", 1));
}