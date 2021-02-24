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
#include "UnityEngine/Color.hpp"
#include "UnityEngine/WaitForEndOfFrame.hpp"
#include "GlobalNamespace/ILightWithId.hpp"
#include "hooks/LightSwitchEventEffect.hpp"

using namespace CustomJSONData;
using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;


DEFINE_CLASS(Il2CppNamespace::WaitThenStartEnumerator);
 
void Il2CppNamespace::WaitThenStartEnumerator::ctor(LightSwitchEventEffect *instance, BeatmapEventType eventType) {
    this->instance = instance;
    this->eventType = eventType;
    this->current = nullptr;
    this->hasWaited = false;
}
 
Il2CppObject* Il2CppNamespace::WaitThenStartEnumerator::get_Current() {
    return current;
}
 
void Il2CppNamespace::WaitThenStartEnumerator::Reset() {}
 
bool Il2CppNamespace::WaitThenStartEnumerator::MoveNext() {
    if (!hasWaited) {
        current = WaitForEndOfFrame::New_ctor();
        hasWaited = true;
        return true; // Continue coroutine
    }
 
    LightColorizer::LSEStart(instance, eventType);

    current = nullptr;
    return false; // Reached end of coroutine
}
 
IEnumerator *WaitThenStart(LightSwitchEventEffect *instance, BeatmapEventType eventType) {
    Il2CppNamespace::WaitThenStartEnumerator *coroutine = CRASH_UNLESS(il2cpp_utils::New<Il2CppNamespace::WaitThenStartEnumerator*>(instance, eventType));
    return reinterpret_cast<IEnumerator*>(coroutine);
}

MAKE_HOOK_OFFSETLESS(LightSwitchEventEffect_Start, void, LightSwitchEventEffect* self) {
    self->StartCoroutine(WaitThenStart(self, self->event));

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
    CRASH_UNLESS(custom_types::Register::RegisterType<Il2CppNamespace::WaitThenStartEnumerator>());

    INSTALL_HOOK_OFFSETLESS(getLogger(), LightSwitchEventEffect_Start, il2cpp_utils::FindMethodUnsafe("", "LightSwitchEventEffect", "Start", 0));
    INSTALL_HOOK_OFFSETLESS(getLogger(), LightSwitchEventEffect_SetColor, il2cpp_utils::FindMethodUnsafe("", "LightSwitchEventEffect", "SetColor", 1));
    INSTALL_HOOK_OFFSETLESS(getLogger(), LightSwitchEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger, il2cpp_utils::FindMethodUnsafe("", "LightSwitchEventEffect", "HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger", 1));
}