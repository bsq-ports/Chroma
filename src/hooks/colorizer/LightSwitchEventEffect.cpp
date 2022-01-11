#include "Chroma.hpp"
#include "ChromaController.hpp"

#include <vector>
#include <optional>
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
#include "UnityEngine/GameObject.hpp"
#include "hooks/LightSwitchEventEffect.hpp"
#include "lighting/LightIDTableManager.hpp"

#include <experimental/coroutine>

using namespace CustomJSONData;
using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;
using namespace custom_types::Helpers;



custom_types::Helpers::Coroutine WaitThenStartLight(LightSwitchEventEffect *instance, BeatmapEventType eventType) {
    co_yield reinterpret_cast<IEnumerator*>(CRASH_UNLESS(WaitForEndOfFrame::New_ctor()));

    ChromaLightSwitchEventEffect* newEffect = instance->get_gameObject()->AddComponent<ChromaLightSwitchEventEffect*>();
    newEffect->CopyValues(instance);
    Object::Destroy(instance);

    co_return;
}

MAKE_HOOK_MATCH(LightSwitchEventEffect_Start,
                &LightSwitchEventEffect::Start,
                void, LightSwitchEventEffect* self) {
    LightSwitchEventEffect_Start(self);

    // Do nothing if Chroma shouldn't run
    if (!ChromaController::GetChromaLegacy() && !ChromaController::DoChromaHooks()) {
        return;
    }

    auto* coro = custom_types::Helpers::CoroutineHelper::New(WaitThenStartLight(self, self->event));

    self->StartCoroutine(reinterpret_cast<IEnumerator*>(coro));

}


MAKE_HOOK_MATCH(LightSwitchEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger,
                &LightSwitchEventEffect::HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger,
                void, LightSwitchEventEffect* self, BeatmapEventData* beatmapEventData) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::GetChromaLegacy() && !ChromaController::DoChromaHooks()) {
        LightSwitchEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
        return;
    }

    // Forward call to avoid make delegate
    reinterpret_cast<ChromaLightSwitchEventEffect*>(self)->HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(beatmapEventData);
}

void LightSwitchEventEffectHook(Logger& logger) {
    INSTALL_HOOK_ORIG(logger, LightSwitchEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger);
    INSTALL_HOOK(logger, LightSwitchEventEffect_Start);
}

ChromaInstallHooks(LightSwitchEventEffectHook)