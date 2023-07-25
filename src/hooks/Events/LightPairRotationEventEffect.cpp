#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "utils/ChromaUtils.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/BeatmapEventData.hpp"
#include "GlobalNamespace/BasicBeatmapEventType.hpp"
#include "GlobalNamespace/LightPairRotationEventEffect.hpp"
#include "GlobalNamespace/LightPairRotationEventEffect_RotationData.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Transform.hpp"
#include "utils/ChromaUtils.hpp"
#include "lighting/ChromaEventData.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Chroma;
using namespace ChromaUtils;

static BasicBeatmapEventData* LastLightPairRotationEventEffectData;

MAKE_HOOK_MATCH(LightPairRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger,
                &LightPairRotationEventEffect::HandleBeatmapEvent, void, LightPairRotationEventEffect* self,
                BasicBeatmapEventData* beatmapEventData) {
  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    LightPairRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
    return;
  }

  if (beatmapEventData->basicBeatmapEventType == self->eventL ||
      beatmapEventData->basicBeatmapEventType == self->eventR) {
    LastLightPairRotationEventEffectData = beatmapEventData;
  }

  //    getLogger().debug("Doing lights");
  LightPairRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
  //    getLogger().debug("Did the custom lights");

  LastLightPairRotationEventEffectData = nullptr;
}

MAKE_HOOK_MATCH(LightPairRotationEventEffect_UpdateRotationData, &LightPairRotationEventEffect::UpdateRotationData,
                void, LightPairRotationEventEffect* self, int beatmapEventDataValue,
                LightPairRotationEventEffect::RotationData* rotationData, float startRotationOffset, float direction) {
  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    LightPairRotationEventEffect_UpdateRotationData(self, beatmapEventDataValue, rotationData, startRotationOffset,
                                                    direction);
    return;
  }

  auto beatmapEventData = LastLightPairRotationEventEffectData;

  auto chromaIt = ChromaEventDataManager::ChromaEventDatas.find(beatmapEventData);

  // Not found
  if (chromaIt == ChromaEventDataManager::ChromaEventDatas.end()) {
    LightPairRotationEventEffect_UpdateRotationData(self, beatmapEventDataValue, rotationData, startRotationOffset,
                                                    direction);
    return;
  }

  auto const& chromaData = chromaIt->second;

  bool isLeftEvent = beatmapEventData->basicBeatmapEventType == self->eventL;
  // rotationData
  LightPairRotationEventEffect::RotationData* customRotationData =
      isLeftEvent ? self->rotationDataL : self->rotationDataR;

  bool lockPosition = chromaData.LockPosition;
  float precisionSpeed = chromaData.Speed.value_or(beatmapEventData->value);
  std::optional<int> dir = chromaData.Direction;

  if (dir) {
    switch (*dir) {
    case 0:
      direction = isLeftEvent ? -1.0f : 1.0f;
      break;

    case 1:
      direction = isLeftEvent ? 1.0f : -1.0f;
      break;
    }
  }

  static auto QuaternionEulerMPtr = FPtrWrapper<static_cast<UnityEngine::Quaternion (*)(UnityEngine::Vector3)>(
      &UnityEngine::Quaternion::Euler)>::get();

  // getLogger().debug("The time is: %d", beatmapEventData->time);
  if (beatmapEventData->value == 0) {
    customRotationData->enabled = false;
    if (!lockPosition) {
      customRotationData->rotationAngle = customRotationData->startRotationAngle;
      customRotationData->transform->set_localRotation(Sombrero::QuaternionMultiply(
          customRotationData->startRotation, QuaternionEulerMPtr(Sombrero::vector3multiply(
                                                 self->rotationVector, customRotationData->startRotationAngle))));
    }
  } else if (beatmapEventData->value > 0) {
    customRotationData->enabled = true;
    customRotationData->rotationSpeed = precisionSpeed * 20.0f * direction;
    // getLogger().debug("Doing rotation speed (%d) %d", beatmapEventData->value, customRotationData->rotationSpeed);
    if (!lockPosition) {
      float rotationAngle = startRotationOffset + customRotationData->startRotationAngle;
      customRotationData->rotationAngle = rotationAngle;
      customRotationData->transform->set_localRotation(Sombrero::QuaternionMultiply(
          customRotationData->startRotation,
          QuaternionEulerMPtr(Sombrero::vector3multiply(self->rotationVector, rotationAngle))));
    }
  }
}

void LightPairRotationEventEffectHook(Logger& logger) {
  INSTALL_HOOK(getLogger(), LightPairRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger);
  INSTALL_HOOK(getLogger(), LightPairRotationEventEffect_UpdateRotationData);
}

ChromaInstallHooks(LightPairRotationEventEffectHook)