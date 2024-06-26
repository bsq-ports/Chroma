#include "Chroma.hpp"
#include "ChromaController.hpp"
#include "ChromaObjectData.hpp"
#include "colorizer/LightColorizer.hpp"
#include "colorizer/Monobehaviours/ChromaParticleEventController.hpp"
#include "utils/ChromaUtils.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/ParticleSystemEventEffect.hpp"
#include "GlobalNamespace/ColorManager.hpp"

#include "UnityEngine/WaitForEndOfFrame.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/GameObject.hpp"

#include "custom-types/shared/coroutine.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace Chroma;
using namespace ChromaUtils;
using namespace custom_types::Helpers;

Coroutine WaitThenStartParticle(ParticleSystemEventEffect* instance, BasicBeatmapEventType eventType) {
  co_yield reinterpret_cast<enumeratorT>(UnityEngine::WaitForEndOfFrame::New_ctor());
  IL2CPP_CATCH_HANDLER(
      instance->get_gameObject()->AddComponent<ChromaParticleEventController*>()->Init(instance, eventType);)
}

MAKE_HOOK_MATCH(ParticleSystemEventEffect_Start, &ParticleSystemEventEffect::Start, void,
                ParticleSystemEventEffect* self) {
  ParticleSystemEventEffect_Start(self);

  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    return;
  }

  // If duplicated, clean up before duping
  auto* oldController = self->GetComponent<ChromaParticleEventController*>();
  if (oldController != nullptr) {
    UnityEngine::Object::Destroy(oldController);
  }

  auto coro = custom_types::Helpers::CoroutineHelper::New(WaitThenStartParticle(self, self->_colorEvent));

  self->StartCoroutine(coro);
}

MAKE_HOOK_MATCH(ParticleSystemEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger,
                &ParticleSystemEventEffect::HandleBeatmapEvent, void, ParticleSystemEventEffect* self,
                BasicBeatmapEventData* beatmapEventData) {
  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    ParticleSystemEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
    return;
  }

  if (beatmapEventData->basicBeatmapEventType == self->_colorEvent) {
    for (auto const& colorizer : ParticleColorizer::GetParticleColorizers(self->_colorEvent)) {
      colorizer->PreviousValue = beatmapEventData->value;
    }
  }

  ParticleSystemEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
}

void ParticleSystemEventEffectHook() {
  INSTALL_HOOK(ChromaLogger::Logger, ParticleSystemEventEffect_Start);
  INSTALL_HOOK(ChromaLogger::Logger, ParticleSystemEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger);
}

ChromaInstallHooks(ParticleSystemEventEffectHook)