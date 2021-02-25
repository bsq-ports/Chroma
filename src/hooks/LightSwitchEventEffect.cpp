#include "Chroma.hpp"

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

custom_types::Helpers::Coroutine WaitForEndFrameCoro() {
    CRASH_UNLESS(WaitForEndOfFrame::New_ctor());
    co_return;
}

custom_types::Helpers::Coroutine WaitThenStart(LightSwitchEventEffect *instance, BeatmapEventType eventType) {
    CRASH_UNLESS(instance);
    co_yield reinterpret_cast<IEnumerator*>(custom_types::Helpers::CoroutineHelper::New(WaitForEndFrameCoro()));
    LightColorizer::LSEStart(instance, eventType);
    co_return;
}

MAKE_HOOK_OFFSETLESS(LightSwitchEventEffect_Start, void, LightSwitchEventEffect* self) {
    auto* coro = custom_types::Helpers::CoroutineHelper::New(WaitThenStart(self, self->event));

    self->StartCoroutine(reinterpret_cast<IEnumerator*>(coro));

    LightSwitchEventEffect_Start(self);
}

MAKE_HOOK_OFFSETLESS(LightSwitchEventEffect_SetColor, void, LightSwitchEventEffect* self, UnityEngine::Color color) {
    if (OverrideLightWithIdActivation){
        auto lights = OverrideLightWithIdActivation.value();
        for (auto & light : lights){
            light->ColorWasSet(color);
        }
    } else {
        LightSwitchEventEffect_SetColor(self, color);
    }
}

MAKE_HOOK_OFFSETLESS(LightSwitchEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger, void, LightSwitchEventEffect* self, CustomBeatmapEventData* beatmapEventData) {
    if (beatmapEventData->type == self->event) {
        LightColorManager::ColorLightSwitch(self, beatmapEventData);
    }

    LightSwitchEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);

    OverrideLightWithIdActivation = std::nullopt;
}

void Chroma::Hooks::LightSwitchEventEffect() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), LightSwitchEventEffect_Start, il2cpp_utils::FindMethodUnsafe("", "LightSwitchEventEffect", "Start", 0));
    INSTALL_HOOK_OFFSETLESS(getLogger(), LightSwitchEventEffect_SetColor, il2cpp_utils::FindMethodUnsafe("", "LightSwitchEventEffect", "SetColor", 1));
    INSTALL_HOOK_OFFSETLESS(getLogger(), LightSwitchEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger, il2cpp_utils::FindMethodUnsafe("", "LightSwitchEventEffect", "HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger", 1));
}