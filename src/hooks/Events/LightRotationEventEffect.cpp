#include <cmath>

#include "Chroma.hpp"
#include "ChromaController.hpp"
#include "ChromaLogger.hpp"
#include "utils/ChromaUtils.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"

#include "GlobalNamespace/LightRotationEventEffect.hpp"

#include "UnityEngine/Random.hpp"
#include "UnityEngine/Space.hpp"
#include "UnityEngine/Transform.hpp"

#include "lighting/ChromaEventData.hpp"
#include "sombrero/shared/RandomUtils.hpp"



using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Chroma;
using namespace ChromaUtils;

MAKE_HOOK_MATCH(LightRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger,
                &LightRotationEventEffect::HandleBeatmapEvent, void, LightRotationEventEffect* self,
                BasicBeatmapEventData* beatmapEventData) {
  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    LightRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
    return;
  }

  

  auto beatmapEventDataOpt = il2cpp_utils::try_cast<CustomJSONData::CustomBeatmapEventData>(beatmapEventData);
  if (!beatmapEventDataOpt) {
    LightRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
    return;
  }

  // Not found


  auto const& chromaData = getLightAD(beatmapEventDataOpt.value()->customData);

  bool isLeftEvent = self->_event == BasicBeatmapEventType::Event12;

  bool lockPosition = chromaData.LockPosition;

  float precisionSpeed = chromaData.Speed.value_or(beatmapEventData->value);

  std::optional<int> dir = chromaData.Direction;

  float direction = NAN;
  if (dir) {
    switch (dir.value()) {
    case 0:
      direction = isLeftEvent ? -1.0F : 1.0F;
      break;
    case 1:
      direction = isLeftEvent ? 1.0F : -1.0F;
      break;
    }
  } else {
    direction = (ChromaController::randomXoshiro() > 0.5F) ? 1.0F : -1.0F;
  }

  if (beatmapEventData->value == 0) {
    self->set_enabled(false);
    if (!lockPosition) {
      self->get_transform()->set_localRotation(self->_startRotation);
    }
  } else if (beatmapEventData->value > 0) {
    self->set_enabled(true);
    self->_rotationSpeed = precisionSpeed * 20.0F * direction;
    if (!lockPosition) {
      auto transform = self->get_transform();
      transform->set_localRotation(self->_startRotation);
      transform->Rotate(self->_rotationVector, ChromaController::randomXoshiro(0.0F, 180.0F), Space::Self);
    }
  }
}

void LightRotationEventEffectHook() {
  INSTALL_HOOK(ChromaLogger::Logger, LightRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger);
}

ChromaInstallHooks(LightRotationEventEffectHook)