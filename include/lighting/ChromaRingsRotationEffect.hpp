#pragma once

#include "UnityEngine/MonoBehaviour.hpp"
#include "GlobalNamespace/TrackLaneRingsRotationEffect.hpp"
#include "GlobalNamespace/TrackLaneRingsManager.hpp"
#include "GlobalNamespace/BasicBeatmapEventType.hpp"
#include "GlobalNamespace/SimpleColorSO.hpp"
#include "GlobalNamespace/MultipliedColorSO.hpp"
#include "GlobalNamespace/TrackLaneRingsManager.hpp"
#include "GlobalNamespace/TrackLaneRing.hpp"
#include <vector>
#include <string>
#include <optional>
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include "main.hpp"

namespace Chroma {
struct ChromaRotationEffect {
public:
  float RotationAngle;

  float RotationStep;

  float RotationFlexySpeed;

  float RotationPropagationSpeed;

  float ProgressPos;

  ChromaRotationEffect(float rotationAngle, float rotationStep, float rotationFlexySpeed,
                       float rotationPropagationSpeed, float progressPos)
      : RotationAngle(rotationAngle), RotationStep(rotationStep), RotationFlexySpeed(rotationFlexySpeed),
        RotationPropagationSpeed(rotationPropagationSpeed), ProgressPos(progressPos) {}

  ChromaRotationEffect() = default;
};
} // namespace Chroma

DECLARE_CLASS_CODEGEN(Chroma, ChromaRingsRotationEffect, GlobalNamespace::TrackLaneRingsRotationEffect) {
public:
  std::vector<ChromaRotationEffect> _activeRingRotationEffects;
  std::vector<ChromaRotationEffect> _ringRotationEffectsPool;

  DECLARE_DEFAULT_CTOR();
  DECLARE_SIMPLE_DTOR();
  DECLARE_INSTANCE_METHOD(void, CopyValues,
                          GlobalNamespace::TrackLaneRingsRotationEffect* trackLaneRingsRotationEffect);

  DECLARE_INSTANCE_METHOD(float, GetFirstRingRotationAngle);
  DECLARE_INSTANCE_METHOD(float, GetFirstRingDestinationRotationAngle);

public:
  float GetFirstRingRotationAngleCpp() {
    static auto GetRotation = il2cpp_utils::il2cpp_type_check::FPtrWrapper<&GlobalNamespace::TrackLaneRing::GetRotation>::get();
    if (!_trackLaneRingsManager->_rings) {
      ChromaLogger::Logger.warn("Rings is null why! {} ", fmt::ptr(_trackLaneRingsManager.ptr()));
      _trackLaneRingsManager->_rings = Array<GlobalNamespace::TrackLaneRing*>::New();
    }

    return GetRotation(_trackLaneRingsManager->_rings.get(0));
  }

  float GetFirstRingDestinationRotationAngleCpp() {
    static auto GetDestinationRotation = il2cpp_utils::il2cpp_type_check::FPtrWrapper<&GlobalNamespace::TrackLaneRing::GetDestinationRotation>::get();

    if (!_trackLaneRingsManager->_rings) {
      ChromaLogger::Logger.warn("Rings is null why! {} ", fmt::ptr(_trackLaneRingsManager.ptr()));
      _trackLaneRingsManager->_rings = Array<GlobalNamespace::TrackLaneRing*>::New();
    }

    return GetDestinationRotation(_trackLaneRingsManager->_rings.get(0));
  }

  DECLARE_INSTANCE_METHOD(void, AddRingRotationEffectF, float angle, float step, float propagationSpeed,
                          float flexySpeed);
  DECLARE_INSTANCE_METHOD(void, AddRingRotationEffect, float angle, float step, int propagationSpeed, float flexySpeed);

  DECLARE_INSTANCE_METHOD(void, SetNewRingManager, GlobalNamespace::TrackLaneRingsManager* trackLaneRingsManager);

  DECLARE_INSTANCE_METHOD(void, Awake);
  DECLARE_INSTANCE_METHOD(void, Start);
  DECLARE_INSTANCE_METHOD(void, FixedUpdate);

  ChromaRotationEffect SpawnRingRotationEffect();
  void RecycleRingRotationEffect(ChromaRotationEffect const& ringRotationEffect);
};