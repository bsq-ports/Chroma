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
  if (!ChromaController::DoChromaHooks()) {
    TrackLaneRingsRotationEffectSpawner_Start(self);
    return;
  }

  static auto* TrackLaneRingsRotationEffectKlass = classof(TrackLaneRingsRotationEffect*);

  if (self->_trackLaneRingsRotationEffect->klass == TrackLaneRingsRotationEffectKlass) {
    auto oldRotationEffect = self->_trackLaneRingsRotationEffect;
    debugSpamLog("Adding component");
    auto* newRotationEffect = oldRotationEffect->get_gameObject()->AddComponent<ChromaRingsRotationEffect*>();
    debugSpamLog("Copyying values now");
    newRotationEffect->CopyValues(oldRotationEffect);

    UnityEngine::Object::Destroy(oldRotationEffect);

    self->_trackLaneRingsRotationEffect = newRotationEffect;
  }
  TrackLaneRingsRotationEffectSpawner_Start(self);
}

template <typename T> T getValueOrDefault(rapidjson::Value* val, std::string const& s, T def) {
  auto v = val->FindMember(s);
  return v != val->MemberEnd() ? v->value.Get<T>() : def;
}

void TriggerRotation(TrackLaneRingsRotationEffect* trackLaneRingsRotationEffect, bool rotRight, float rotation, float rotationStep,
                     float rotationPropagationSpeed, float rotationFlexySpeed) {
  debugSpamLog("DOING TRIGGER ROTATION {}", trackLaneRingsRotationEffect->klass->name);

  auto* chromaRingRotation = static_cast<ChromaRingsRotationEffect*>(trackLaneRingsRotationEffect);

  chromaRingRotation->AddRingRotationEffectF(chromaRingRotation->GetFirstRingDestinationRotationAngleCpp() +
                                                 (rotation * (rotRight ? -1.0F : 1.0F)),
                                             rotationStep, rotationPropagationSpeed, rotationFlexySpeed);
}

// TODO:
// This method is directly ported from TrackLaneRingsRotationEffectSpawner. It is required to be ported since for some
// inexplicable reason using the original method causes CJD or something else to stop loading the map and it just stays
// as limbo. Hopefully with time we can fix that and use that instead
void origHandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(GlobalNamespace::TrackLaneRingsRotationEffectSpawner* self,
                                                                     BasicBeatmapEventData* beatmapEventData) {
  if (beatmapEventData->basicBeatmapEventType != self->_beatmapEventType) {
    return;
  }
  float step = 0.0F;

  int originalRotationStepType = self->_rotationStepType.value__;

  if (originalRotationStepType == TrackLaneRingsRotationEffectSpawner::RotationStepType::Range0ToMax.value__) {
    step = ChromaController::randomXoshiro(0.0F, self->_rotationStep);
  } else if (originalRotationStepType == TrackLaneRingsRotationEffectSpawner::RotationStepType::Range.value__) {
    step = ChromaController::randomXoshiro(-self->_rotationStep, self->_rotationStep);
  } else if (originalRotationStepType == TrackLaneRingsRotationEffectSpawner::RotationStepType::MaxOr0.value__) {
    step = (ChromaController::randomXoshiro() < 0.5F) ? self->_rotationStep : 0.0F;
  }
  debugSpamLog("Track lane klass {}", self->_trackLaneRingsRotationEffect->klass->name);

  static auto* ChromaRingsRotationEffectKlass = classof(ChromaRingsRotationEffect*);

  auto rotationEffect = self->_trackLaneRingsRotationEffect;

  if (ASSIGNMENT_CHECK(ChromaRingsRotationEffectKlass, rotationEffect->klass)) {

    auto* chromaRotation = reinterpret_cast<ChromaRingsRotationEffect*>(rotationEffect.ptr());

    chromaRotation->AddRingRotationEffectF(chromaRotation->GetFirstRingDestinationRotationAngleCpp() +
                                               (self->_rotation * static_cast<float>((ChromaController::randomXoshiro() < 0.5F) ? 1 : -1)),
                                           step, static_cast<float>(self->_rotationPropagationSpeed), self->_rotationFlexySpeed);
  } else {
    rotationEffect->AddRingRotationEffect(rotationEffect->GetFirstRingDestinationRotationAngle() +
                                              (self->_rotation * static_cast<float>((ChromaController::randomXoshiro() < 0.5F) ? 1 : -1)),
                                          step, self->_rotationPropagationSpeed, self->_rotationFlexySpeed);
  }
}

MAKE_HOOK_MATCH(TrackLaneRingsRotationEffectSpawner_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger,
                &TrackLaneRingsRotationEffectSpawner::HandleBeatmapEvent, void, GlobalNamespace::TrackLaneRingsRotationEffectSpawner* self,
                BasicBeatmapEventData* beatmapEventData) {
  if (!ChromaController::DoChromaHooks()) {
    TrackLaneRingsRotationEffectSpawner_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
    return;
  }

  //    debugSpamLog("Track lane rotation effect self {} beat {} and customData {}",
  //    self->BasicBeatmapEventType.value,
  //                      beatmapEventData->type.value,
  //                      beatmapEventData->customData != nullptr && beatmapEventData->customData->value != nullptr ? 0
  //                      : 1);

  if (beatmapEventData->basicBeatmapEventType == self->_beatmapEventType) {
    auto beatmapEventDataOpt = il2cpp_utils::try_cast<CustomBeatmapEventData>(beatmapEventData);

    // Not found
    if (!beatmapEventDataOpt) {
      origHandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
      return;
    }

    auto const& chromaData = getLightAD(beatmapEventDataOpt.value()->customData);

    debugSpamLog("Doing stuff with custom Data ring");

    float rotationStep = 0.0F;

    float originalRotationStep = self->_rotationStep;
    float originalRotation = self->_rotation;
    auto originalRotationPropagationSpeed = static_cast<float>(self->_rotationPropagationSpeed);
    float originalRotationFlexySpeed = self->_rotationFlexySpeed;
    int originalRotationStepType = self->_rotationStepType.value__;

    if (originalRotationStepType == TrackLaneRingsRotationEffectSpawner::RotationStepType::Range0ToMax.value__) {
      rotationStep = ChromaController::randomXoshiro(0.0F, rotationStep);
    } else if (originalRotationStepType == TrackLaneRingsRotationEffectSpawner::RotationStepType::Range.value__) {
      rotationStep = ChromaController::randomXoshiro(-originalRotationStep, originalRotationStep);
    } else if (originalRotationStepType == TrackLaneRingsRotationEffectSpawner::RotationStepType::MaxOr0.value__) {
      rotationStep = (ChromaController::randomXoshiro() < 0.5F) ? originalRotationStep : 0.0F;
    }

    debugSpamLog("Got the data");
    std::string selfName = self->get_name();

    auto nameFilter = chromaData.NameFilter;
    // If not equal with ignore case
    if (nameFilter && stringCompare(selfName, nameFilter.value()) != 0) {
      debugSpamLog("Name filter ignored");
      return;
    }

    auto dir = chromaData.Direction;

    // https://github.com/Aeroluna/Chroma/commit/3900969d3fef1eaeea745bcfc23c61bfbe525586#diff-e83fa5da7e2e725f2cfb2ee5a6d6a085b2065a95e0d4757e01fe3c29f0fa4024
    bool rotRight = false;
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
      debugSpamLog("Reset spawn, returning");
      TriggerRotation(self->_trackLaneRingsRotationEffect, rotRight, originalRotation, 0, 50, 50);
      return;
    }

    debugSpamLog("Getting the last values");

    float step = chromaData.Step.value_or(rotationStep);
    float prop = chromaData.Prop.value_or(originalRotationPropagationSpeed);
    float speed = chromaData.Speed.value_or(originalRotationFlexySpeed);
    float rotation = chromaData.Rotation.value_or(originalRotation);

    float stepMult = chromaData.SpeedMult;
    float propMult = chromaData.PropMult;
    float speedMult = chromaData.SpeedMult;

    TriggerRotation(self->_trackLaneRingsRotationEffect, rotRight, rotation, step * stepMult, prop * propMult, speed * speedMult);
    debugSpamLog("Finished spawn, returning");
    return;
  }

  debugSpamLog("Not a custom beat map");
  origHandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(
      self,
      beatmapEventData); //        TrackLaneRingsRotationEffectSpawner_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self,
                         //        beatmapEventData);
}

void TrackLaneRingsRotationEffectSpawnerHook() {
  INSTALL_HOOK(ChromaLogger::Logger, TrackLaneRingsRotationEffectSpawner_Start);
  INSTALL_HOOK(ChromaLogger::Logger, TrackLaneRingsRotationEffectSpawner_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger);
  //    INSTALL_HOOK_OFFSETLESS(ChromaLogger::Logger, SaberManager_Finalize, il2cpp_utils::FindMethodUnsafe("System", "Object",
  //    "Finalize", 0));
}

ChromaInstallHooks(TrackLaneRingsRotationEffectSpawnerHook)