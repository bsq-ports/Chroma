#include "ChromaController.hpp"
#include "main.hpp"
#include "Chroma.hpp"

#include "lighting/ChromaEventData.hpp"

#include "GlobalNamespace/TrackLaneRingsPositionStepEffectSpawner.hpp"
#include "GlobalNamespace/BasicBeatmapEventData.hpp"
#include "GlobalNamespace/TrackLaneRingsManager.hpp"
#include "GlobalNamespace/TrackLaneRing.hpp"

using namespace Chroma;
using namespace GlobalNamespace;

static float GetPrecisionStep(float const defaultF, GlobalNamespace::BasicBeatmapEventData* beatmapEventData) {
  auto const& chromaData = getLightAD(beatmapEventData);
  if (chromaData && chromaData->Step) {

    return chromaData->Step.value();
  }

  return defaultF;
}

static float GetPrecisionSpeed(float const defaultF, GlobalNamespace::BasicBeatmapEventData* beatmapEventData) {
  auto chromaData = getLightAD(beatmapEventData);

  if (chromaData && chromaData->Speed) {
    return chromaData->Speed.value();
  }

  return defaultF;
}

// Aero why do you have to use transpilers for everything damn it? Just rewrite the method
MAKE_HOOK_MATCH(TrackLaneRingsPositionStepEffectSpawner_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger,
                &TrackLaneRingsPositionStepEffectSpawner::HandleBeatmapEvent, void,
                GlobalNamespace::TrackLaneRingsPositionStepEffectSpawner* self, GlobalNamespace::BasicBeatmapEventData* beatmapEventData) {
  // Essentially, here we cancel the original method. DO NOT call it IF it's a Chroma map
  if (!ChromaController::DoChromaHooks()) {
    TrackLaneRingsPositionStepEffectSpawner_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
    return;
  }

  float num = (beatmapEventData->sameTypeIndex % 2 == 0) ? self->_maxPositionStep : self->_minPositionStep;

  num = GetPrecisionStep(num, beatmapEventData);

  auto rings = self->_trackLaneRingsManager->_rings;
  for (int i = 0; i < rings.size(); i++) {
    float destPosZ = static_cast<float>(i) * num;
    static auto SetPosition = FPtrWrapper<&GlobalNamespace::TrackLaneRing::SetPosition>::get();

    float moveSpeed = GetPrecisionSpeed(self->_moveSpeed, beatmapEventData);
    SetPosition(rings[i], destPosZ, moveSpeed);
  }
}

void TrackLaneRingsPositionStepEffectSpawnerHook() {
  INSTALL_HOOK(ChromaLogger::Logger, TrackLaneRingsPositionStepEffectSpawner_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger);
  //    INSTALL_HOOK_OFFSETLESS(ChromaLogger::Logger, SaberManager_Finalize, il2cpp_utils::FindMethodUnsafe("System", "Object",
  //    "Finalize", 0));
}

ChromaInstallHooks(TrackLaneRingsPositionStepEffectSpawnerHook)