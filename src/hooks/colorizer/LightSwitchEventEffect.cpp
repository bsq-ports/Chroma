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



custom_types::Helpers::Coroutine WaitThenStartLight(LightSwitchEventEffect *instance, BeatmapEventType eventType) {
    co_yield reinterpret_cast<IEnumerator*>(CRASH_UNLESS(WaitForEndOfFrame::New_ctor()));
    LightColorizer::New(instance, eventType);
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

MAKE_HOOK_MATCH(LightSwitchEventEffect_OnDestroy,
                &LightSwitchEventEffect::OnDestroy,
                void, LightSwitchEventEffect* self) {
    LightSwitchEventEffect_OnDestroy(self);

    // Do nothing if Chroma shouldn't run
    if (!ChromaController::GetChromaLegacy() && !ChromaController::DoChromaHooks()) {
        return;
    }

    LightColorizer::Colorizers.erase(self->event);

}

MAKE_HOOK_MATCH(LightSwitchEventEffect_SetColor,
                &LightSwitchEventEffect::SetColor,
                void, LightSwitchEventEffect* self, UnityEngine::Color color) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::GetChromaLegacy() && !ChromaController::DoChromaHooks()) {
        LightSwitchEventEffect_SetColor(self, color);
        return;
    }


    if (LightSwitchEventEffectHolder::LightIDOverride) {
        auto const& lightOverride = LightSwitchEventEffectHolder::LightIDOverride.value();
        auto const& lights = LightColorizer::GetLightColorizer(self->event)->Lights;

        int type = self->event;
        std::vector<int> newIds;
        newIds.reserve(lightOverride.size());

        for (auto id : lightOverride) {
            auto newId = LightIDTableManager::GetActiveTableValue(type, id);

            newIds.push_back(newId.value_or(id));
        }


        for (auto id : newIds) {
            if (id < 0 || id >= lights.size()) {
                getLogger().warning("Type %d does not contain id %d", type, id);
            } else {
                auto l = lights.at(id);
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
        auto const&  lights = LightSwitchEventEffectHolder::LegacyLightOverride.value();

        for (auto &light : lights) {
            if (!light) continue;
            light->ColorWasSet(color);
        }

        LightSwitchEventEffectHolder::LegacyLightOverride = std::nullopt;

        return;
    }

    LightSwitchEventEffect_SetColor(self, color);

}

MAKE_HOOK_MATCH(LightSwitchEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger,
                &LightSwitchEventEffect::HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger,
                void, LightSwitchEventEffect* self, BeatmapEventData* beatmapEventData) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::GetChromaLegacy() && !ChromaController::DoChromaHooks()) {
        LightSwitchEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
        return;
    }

    if (beatmapEventData->type == self->event) {
        LightColorManager::ColorLightSwitch(beatmapEventData);
    }

    LightSwitchEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
}

void LightSwitchEventEffectHook(Logger& logger) {
    INSTALL_HOOK(logger, LightSwitchEventEffect_SetColor);
    INSTALL_HOOK(logger, LightSwitchEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger);
    INSTALL_HOOK(logger, LightSwitchEventEffect_Start);
    INSTALL_HOOK(logger, LightSwitchEventEffect_OnDestroy);
}

ChromaInstallHooks(LightSwitchEventEffectHook)