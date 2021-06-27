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
    co_yield reinterpret_cast<IEnumerator*>(CRASH_UNLESS(WaitForEndOfFrame::New_ctor()));
    LightColorizer::New(instance, eventType);
    co_return;
}

MAKE_HOOK_OFFSETLESS(colorizer_LightSwitchEventEffect_Start, void, LightSwitchEventEffect* self) {
    colorizer_LightSwitchEventEffect_Start(self);

    // Do nothing if Chroma shouldn't run
    if (!ChromaController::GetChromaLegacy() && !ChromaController::DoChromaHooks()) {
        return;
    }

    auto* coro = custom_types::Helpers::CoroutineHelper::New(WaitThenStart(self, self->event));

    self->StartCoroutine(reinterpret_cast<IEnumerator*>(coro));

}

MAKE_HOOK_OFFSETLESS(colorizer_LightSwitchEventEffect_OnDestroy, void, LightSwitchEventEffect* self) {
    colorizer_LightSwitchEventEffect_OnDestroy(self);

    // Do nothing if Chroma shouldn't run
    if (!ChromaController::GetChromaLegacy() && !ChromaController::DoChromaHooks()) {
        return;
    }

    LightColorizer::Colorizers.erase(self->event);

}

void Chroma::Hooks::LightSwitchEventEffect() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), colorizer_LightSwitchEventEffect_Start, il2cpp_utils::FindMethodUnsafe("", "LightSwitchEventEffect", "Start", 0));
    INSTALL_HOOK_OFFSETLESS(getLogger(), colorizer_LightSwitchEventEffect_OnDestroy, il2cpp_utils::FindMethodUnsafe("", "LightSwitchEventEffect", "OnDestroy", 0));
}