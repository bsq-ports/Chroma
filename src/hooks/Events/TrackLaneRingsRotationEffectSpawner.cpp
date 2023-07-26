#include "main.hpp"
#include "Chroma.hpp"
#include "utils/ChromaUtils.hpp"
#include "GlobalNamespace/TrackLaneRingsRotationEffectSpawner.hpp"
#include "GlobalNamespace/TrackLaneRingsRotationEffect.hpp"
#include "lighting/ChromaRingsRotationEffect.hpp"
#include "UnityEngine/Random.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Object.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"
#include <cstring>
#include "ChromaController.hpp"
#include "lighting/ChromaEventData.hpp"

using namespace Chroma;
using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace ChromaUtils;

MAKE_HOOK_MATCH(TrackLaneRingsRotationEffectSpawner_Start, &TrackLaneRingsRotationEffectSpawner::Start, void,
                GlobalNamespace::TrackLaneRingsRotationEffectSpawner* self) {
  static auto contextLogger = getLogger().WithContext(Chroma::ChromaLogger::TrackLaneRings);
  if (!ChromaController::DoChromaHooks()) {
    TrackLaneRingsRotationEffectSpawner_Start(self);
    return;
  }

  static auto* TrackLaneRingsRotationEffectKlass = classof(TrackLaneRingsRotationEffect*);

  if (self->trackLaneRingsRotationEffect->klass == TrackLaneRingsRotationEffectKlass) {
    auto* oldRotationEffect = self->trackLaneRingsRotationEffect;
    debugSpamLog(contextLogger, "Adding component");
    auto* newRotationEffect = oldRotationEffect->get_gameObject()->AddComponent<ChromaRingsRotationEffect*>();
    debugSpamLog(contextLogger, "Copyying values now");
    newRotationEffect->CopyValues(oldRotationEffect);

    UnityEngine::Object::Destroy(oldRotationEffect);

    self->trackLaneRingsRotationEffect = newRotationEffect;
  }
  TrackLaneRingsRotationEffectSpawner_Start(self);
}

template <typename T> T getValueOrDefault(rapidjson::Value* val, std::string const& s, T def) {
  auto v = val->FindMember(s);
  return v != val->MemberEnd() ? v->value.Get<T>() : def;
}

void TriggerRotation(TrackLaneRingsRotationEffect* trackLaneRingsRotationEffect, bool rotRight, float rotation,
                     float rotationStep, float rotationPropagationSpeed, float rotationFlexySpeed) {
  static auto contextLogger = getLogger().WithContext(Chroma::ChromaLogger::TrackLaneRings);
  debugSpamLog(contextLogger, "DOING TRIGGER ROTATION %s", trackLaneRingsRotationEffect->klass->name);

  auto* chromaRingRotation = static_cast<ChromaRingsRotationEffect*>(trackLaneRingsRotationEffect);

  chromaRingRotation->AddRingRotationEffectF(chromaRingRotation->GetFirstRingDestinationRotationAngleCpp() +
                                                 (rotation * (rotRight ? -1.0F : 1.0F)),
                                             rotationStep, rotationPropagationSpeed, rotationFlexySpeed);
}

// TODO:
// This method is directly ported from TrackLaneRingsRotationEffectSpawner. It is required to be ported since for some
// inexplicable reason using the original method causes CJD or something else to stop loading the map and it just stays
// as limbo. Hopefully with time we can fix that and use that instead
void origHandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(
    GlobalNamespace::TrackLaneRingsRotationEffectSpawner* self, BasicBeatmapEventData* beatmapEventData) {
  static auto contextLogger = getLogger().WithContext(Chroma::ChromaLogger::TrackLaneRings);
  if (beatmapEventData->basicBeatmapEventType != self->beatmapEventType) {
    return;
  }
  float step = 0.0F;

  int originalRotationStepType = (int)self->rotationStepType;

  if (originalRotationStepType == TrackLaneRingsRotationEffectSpawner::RotationStepType::Range0ToMax) {
    step = ChromaController::randomXoshiro(0.0F, self->rotationStep);
  } else if (originalRotationStepType == TrackLaneRingsRotationEffectSpawner::RotationStepType::Range) {
    step = ChromaController::randomXoshiro(-self->rotationStep, self->rotationStep);
  } else if (originalRotationStepType == TrackLaneRingsRotationEffectSpawner::RotationStepType::MaxOr0) {
    step = (ChromaController::randomXoshiro() < 0.5F) ? self->rotationStep : 0.0F;
  }
  debugSpamLog(contextLogger, "Track lane klass %s", self->trackLaneRingsRotationEffect->klass->name);

  static auto* ChromaRingsRotationEffectKlass = classof(ChromaRingsRotationEffect*);

  auto* rotationEffect = self->trackLaneRingsRotationEffect;

  if (ASSIGNMENT_CHECK(ChromaRingsRotationEffectKlass, self->trackLaneRingsRotationEffect->klass)) {

    auto* chromaRotation = reinterpret_cast<ChromaRingsRotationEffect*>(rotationEffect);

    chromaRotation->AddRingRotationEffectF(
        chromaRotation->GetFirstRingDestinationRotationAngleCpp() +
            self->rotation * static_cast<float>((ChromaController::randomXoshiro() < 0.5F) ? 1 : -1),
        step, static_cast<float>(self->rotationPropagationSpeed), self->rotationFlexySpeed);
  } else {
    rotationEffect->AddRingRotationEffect(
        rotationEffect->GetFirstRingDestinationRotationAngle() +
            self->rotation * static_cast<float>((ChromaController::randomXoshiro() < 0.5F) ? 1 : -1),
        step, self->rotationPropagationSpeed, self->rotationFlexySpeed);
  }
}

MAKE_HOOK_MATCH(TrackLaneRingsRotationEffectSpawner_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger,
                &TrackLaneRingsRotationEffectSpawner::HandleBeatmapEvent, void,
                GlobalNamespace::TrackLaneRingsRotationEffectSpawner* self, BasicBeatmapEventData* beatmapEventData) {
  if (!ChromaController::DoChromaHooks()) {
    TrackLaneRingsRotationEffectSpawner_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self,
                                                                                                    beatmapEventData);
    return;
  }

  //    debugSpamLog(contextLogger, "Track lane rotation effect self %d beat %d and customData %d",
  //    self->BasicBeatmapEventType.value,
  //                      beatmapEventData->type.value,
  //                      beatmapEventData->customData != nullptr && beatmapEventData->customData->value != nullptr ? 0
  //                      : 1);
  static auto contextLogger = getLogger().WithContext(Chroma::ChromaLogger::TrackLaneRings);

  if (beatmapEventData->basicBeatmapEventType == self->beatmapEventType) {
    auto chromaIt = ChromaEventDataManager::ChromaEventDatas.find(beatmapEventData);

    // Not found
    if (chromaIt == ChromaEventDataManager::ChromaEventDatas.end()) {
      origHandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
      return;
    }

    auto const& chromaData = chromaIt->second;

    debugSpamLog(contextLogger, "Doing stuff with custom Data ring");

    float rotationStep = 0.0F;

    float originalRotationStep = self->rotationStep;
    float originalRotation = self->rotation;
    auto originalRotationPropagationSpeed = static_cast<float>(self->rotationPropagationSpeed);
    float originalRotationFlexySpeed = self->rotationFlexySpeed;
    int originalRotationStepType = (int)self->rotationStepType;

    if (originalRotationStepType == TrackLaneRingsRotationEffectSpawner::RotationStepType::Range0ToMax) {
      rotationStep = ChromaController::randomXoshiro(0.0F, rotationStep);
    } else if (originalRotationStepType == TrackLaneRingsRotationEffectSpawner::RotationStepType::Range) {
      rotationStep = ChromaController::randomXoshiro(-originalRotationStep, originalRotationStep);
    } else if (originalRotationStepType == TrackLaneRingsRotationEffectSpawner::RotationStepType::MaxOr0) {
      rotationStep = (ChromaController::randomXoshiro() < 0.5F) ? originalRotationStep : 0.0F;
    }

    debugSpamLog(contextLogger, "Got the data");
    std::string selfName = self->get_name();

    auto nameFilter = chromaData.NameFilter;
    // If not equal with ignore case
    if (nameFilter && stringCompare(selfName, nameFilter.value()) != 0) {
      debugSpamLog(contextLogger, "Name filter ignored");
      return;
    }

    auto dir = chromaData.Direction;

    // https://github.com/Aeroluna/Chroma/commit/3900969d3fef1eaeea745bcfc23c61bfbe525586#diff-e83fa5da7e2e725f2cfb2ee5a6d6a085b2065a95e0d4757e01fe3c29f0fa4024
    bool rotRight = 0;
    if (!dir) {
      rotRight = ChromaController::randomXoshiro() < 0.5F;
    } else {
      rotRight = dir.value() == 1;
    }

    auto counterSpinV = chromaData.CounterSpin;
    if (counterSpinV && counterSpinV.value()) {
      if (selfName.find("Big") == std::string::npos) {
        rotRight = !rotRight;
      }
    }

    auto reset = chromaData.Reset;
    if (reset && reset.value()) {
      debugSpamLog(contextLogger, "Reset spawn, returning");
      TriggerRotation(self->trackLaneRingsRotationEffect, rotRight, originalRotation, 0, 50, 50);
      return;
    }

    debugSpamLog(contextLogger, "Getting the last values");

    float step = chromaData.Step.value_or(rotationStep);
    float prop = chromaData.Prop.value_or(originalRotationPropagationSpeed);
    float speed = chromaData.Speed.value_or(originalRotationFlexySpeed);
    float rotation = chromaData.Rotation.value_or(originalRotation);

    float stepMult = chromaData.SpeedMult;
    float propMult = chromaData.PropMult;
    float speedMult = chromaData.SpeedMult;

    TriggerRotation(self->trackLaneRingsRotationEffect, rotRight, rotation, step * stepMult, prop * propMult,
                    speed * speedMult);
    debugSpamLog(contextLogger, "Finished spawn, returning");
    return;
  }

  debugSpamLog(contextLogger, "Not a custom beat map");
  origHandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(
      self,
      beatmapEventData); //        TrackLaneRingsRotationEffectSpawner_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self,
                         //        beatmapEventData);
}

void TrackLaneRingsRotationEffectSpawnerHook(Logger& logger) {
  INSTALL_HOOK(logger, TrackLaneRingsRotationEffectSpawner_Start);
  INSTALL_HOOK(logger, TrackLaneRingsRotationEffectSpawner_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger);
  //    INSTALL_HOOK_OFFSETLESS(getLogger(), SaberManager_Finalize, il2cpp_utils::FindMethodUnsafe("System", "Object",
  //    "Finalize", 0));
}

ChromaInstallHooks(TrackLaneRingsRotationEffectSpawnerHook)