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
#include "GlobalNamespace/BeatmapObjectCallbackController.hpp"
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

    auto* newEffect = instance->get_gameObject()->AddComponent<ChromaLightSwitchEventEffect*>();
    newEffect->CopyValues(instance);
    Object::Destroy(instance);

    co_return;
}

MAKE_HOOK_MATCH(LightSwitchEventEffect_Awake,
                &LightSwitchEventEffect::Awake,
                void, LightSwitchEventEffect* self) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::GetChromaLegacy() && !ChromaController::DoChromaHooks()) {
        return LightSwitchEventEffect_Awake(self);
    }

    // override method to do nothing
}

MAKE_HOOK_MATCH(LightSwitchEventEffect_Start,
                &LightSwitchEventEffect::Start,
                void, LightSwitchEventEffect* self) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::GetChromaLegacy() && !ChromaController::DoChromaHooks()) {
        return LightSwitchEventEffect_Start(self);
    }

    static auto ChromaLightSwitchEventEffectKlass = classof(ChromaLightSwitchEventEffect*);

    if (self->klass == ChromaLightSwitchEventEffectKlass) return;

    auto coro = custom_types::Helpers::CoroutineHelper::New(WaitThenStartLight(self, self->event));

    self->StartCoroutine(coro);
}

MAKE_HOOK_MATCH(BeatmapObjectCallbackController_SendBeatmapEventDidTriggerEvent,
                &BeatmapObjectCallbackController::SendBeatmapEventDidTriggerEvent,
                void,
                BeatmapObjectCallbackController* self, BeatmapEventData* eventData) {
    BeatmapObjectCallbackController_SendBeatmapEventDidTriggerEvent(self, eventData);

    // Do nothing if Chroma shouldn't run
    if (!ChromaController::GetChromaLegacy() && !ChromaController::DoChromaHooks()) {
        return;
    }

    // I don't want to deal with delegates
    for (auto const& _lightSwitchEventEffect :ChromaLightSwitchEventEffect::livingLightSwitch) {
        _lightSwitchEventEffect->HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(eventData);
    }
}

void LightSwitchEventEffectHook(Logger& logger) {
    INSTALL_HOOK(logger, BeatmapObjectCallbackController_SendBeatmapEventDidTriggerEvent);

    INSTALL_HOOK(logger, LightSwitchEventEffect_Start);
    INSTALL_HOOK(logger, LightSwitchEventEffect_Awake);
}

ChromaInstallHooks(LightSwitchEventEffectHook)