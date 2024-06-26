#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "colorizer/LightColorizer.hpp"

#include <vector>
#include <optional>
#include "custom-json-data/shared/CustomBeatmapData.h"
#include "System/Collections/IEnumerator.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/WaitForEndOfFrame.hpp"
#include "GlobalNamespace/ILightWithId.hpp"
#include "GlobalNamespace/BeatmapCallbacksController.hpp"
#include "GlobalNamespace/CallbacksInTime.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"
#include "UnityEngine/GameObject.hpp"
#include "hooks/LightSwitchEventEffect.hpp"
#include "lighting/LightIDTableManager.hpp"

using namespace CustomJSONData;
using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;
using namespace custom_types::Helpers;

BeatmapCallbacksController* beatmapCallbacksController;

custom_types::Helpers::Coroutine WaitThenStartLight(LightSwitchEventEffect* instance, BasicBeatmapEventType eventType) {
  co_yield reinterpret_cast<IEnumerator*>(CRASH_UNLESS(WaitForEndOfFrame::New_ctor()));

  auto* newEffect = instance->get_gameObject()->AddComponent<ChromaLightSwitchEventEffect*>();
  newEffect->CopyValues(instance);

  IL2CPP_CATCH_HANDLER(Object::Destroy(instance);)

  co_return;
}

MAKE_HOOK_MATCH(LightSwitchEventEffect_Start, &LightSwitchEventEffect::Start, void, LightSwitchEventEffect* self) {
  static auto* ChromaLightSwitchEventEffectKlass = classof(ChromaLightSwitchEventEffect*);

  if (self->klass == ChromaLightSwitchEventEffectKlass) {
    return;
  }

  // Do nothing if Chroma shouldn't run
  if (!ChromaController::GetChromaLegacy() && !ChromaController::DoChromaHooks()) {
    return LightSwitchEventEffect_Start(self);
  }

  auto coro = custom_types::Helpers::CoroutineHelper::New(WaitThenStartLight(self, self->_event));

  self->StartCoroutine(coro);
}

MAKE_HOOK_MATCH(BeatmapCallbacksController_ManualUpdate, &BeatmapCallbacksController::ManualUpdate, void,
                BeatmapCallbacksController* self, float songTime) {
  // Do nothing if Chroma shouldn't run
  if (!ChromaController::GetChromaLegacy() && !ChromaController::DoChromaHooks()) {
    return BeatmapCallbacksController_ManualUpdate(self, songTime);
  }

  if (self != beatmapCallbacksController) {
    beatmapCallbacksController = self;

    // I don't want to deal with delegates

    auto* basicEvents = CustomJSONData::CustomBeatmapDataCallbackWrapper::New_ctor();
    basicEvents->controller = self;
    basicEvents->BasicBeatmapEventType = csTypeOf(BasicBeatmapEventData*);
    basicEvents->redirectEvent = [](auto* /*controller*/, BeatmapDataItem* item) {
      for (auto const& _lightSwitchEventEffect : ChromaLightSwitchEventEffect::livingLightSwitch) {
        _lightSwitchEventEffect->HandleEvent(static_cast<BasicBeatmapEventData*>(item));
      }
    };

    auto* boostEvents = CustomJSONData::CustomBeatmapDataCallbackWrapper::New_ctor();
    boostEvents->controller = self;
    boostEvents->BasicBeatmapEventType = csTypeOf(ColorBoostBeatmapEventData*);
    boostEvents->redirectEvent = [](auto* /*controller*/, BeatmapDataItem* item) {
      for (auto const& _lightSwitchEventEffect : ChromaLightSwitchEventEffect::livingLightSwitch) {
        _lightSwitchEventEffect->HandleBoostEvent(static_cast<ColorBoostBeatmapEventData*>(item));
      }
    };

    self->_callbacksInTimes->get_Item(0)->AddCallback(basicEvents);
    self->_callbacksInTimes->get_Item(0)->AddCallback(boostEvents);
  }

  BeatmapCallbacksController_ManualUpdate(self, songTime);
}

void LightSwitchEventEffectHook() {
  INSTALL_HOOK(ChromaLogger::Logger, BeatmapCallbacksController_ManualUpdate);

  INSTALL_HOOK(ChromaLogger::Logger, LightSwitchEventEffect_Start);
}

ChromaInstallHooks(LightSwitchEventEffectHook)