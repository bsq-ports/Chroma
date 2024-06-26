#include "Chroma.hpp"
#include "lighting/ChromaRingsRotationEffect.hpp"
#include "GlobalNamespace/TrackLaneRingsRotationEffect.hpp"
#include "GlobalNamespace/TrackLaneRing.hpp"

using namespace Chroma;

DEFINE_TYPE(Chroma, ChromaRingsRotationEffect);

void ChromaRingsRotationEffect::AddRingRotationEffect(float angle, float step, int propagationSpeed, float flexySpeed) {
  AddRingRotationEffectF(angle, step, static_cast<float>(propagationSpeed), flexySpeed);
}

void ChromaRingsRotationEffect::AddRingRotationEffectF(float angle, float step, float propagationSpeed,
                                                       float flexySpeed) {
  ChromaRotationEffect ringRotationEffect = SpawnRingRotationEffect();
  ringRotationEffect.ProgressPos = 0;
  ringRotationEffect.RotationAngle = angle;
  ringRotationEffect.RotationStep = step;
  ringRotationEffect.RotationPropagationSpeed = propagationSpeed;
  ringRotationEffect.RotationFlexySpeed = flexySpeed;
  _activeRingRotationEffects.emplace_back(ringRotationEffect);
}

void ChromaRingsRotationEffect::SetNewRingManager(GlobalNamespace::TrackLaneRingsManager* trackLaneRingsManager) {
  this->_trackLaneRingsManager = _trackLaneRingsManager;
}

void ChromaRingsRotationEffect::CopyValues(
    GlobalNamespace::TrackLaneRingsRotationEffect* trackLaneRingsRotationEffect) {
  this->_trackLaneRingsManager = trackLaneRingsRotationEffect->_trackLaneRingsManager;
  this->_startupRotationAngle = trackLaneRingsRotationEffect->_startupRotationAngle;
  this->_startupRotationStep = trackLaneRingsRotationEffect->_startupRotationStep;
  this->_startupRotationPropagationSpeed = trackLaneRingsRotationEffect->_startupRotationPropagationSpeed;
  this->_startupRotationFlexySpeed = trackLaneRingsRotationEffect->_startupRotationFlexySpeed;
}

void ChromaRingsRotationEffect::Awake() {
  int poolCount = 20;
  _activeRingRotationEffects = std::vector<ChromaRotationEffect>();
  _activeRingRotationEffects.reserve(poolCount);
  _ringRotationEffectsPool = std::vector<ChromaRotationEffect>(poolCount);
}

void ChromaRingsRotationEffect::Start() {
  AddRingRotationEffectF(_startupRotationAngle, _startupRotationStep, static_cast<float>(_startupRotationPropagationSpeed),
                         _startupRotationFlexySpeed);
}

void ChromaRingsRotationEffect::FixedUpdate() {
  if (!_activeRingRotationEffects.empty()) {
    auto rings = _trackLaneRingsManager->_rings;

    if (!rings) {
      ChromaLogger::Logger.warn("Rings is null why! {} ", fmt::ptr(_trackLaneRingsManager.ptr()));
      _trackLaneRingsManager->_rings = ArrayW<UnityW<GlobalNamespace::TrackLaneRing>>();
    }

    static auto SetDestRotation = FPtrWrapper<&GlobalNamespace::TrackLaneRing::SetDestRotation>::get();

    // Reverse iterate so we can delete while iterating
    for (auto it = _activeRingRotationEffects.rbegin(); it != _activeRingRotationEffects.rend(); it++) {
      ChromaRotationEffect& ringRotationEffect = *it;
      auto num = static_cast<long>(ringRotationEffect.ProgressPos);
      auto progressPos = ringRotationEffect.ProgressPos += ringRotationEffect.RotationPropagationSpeed;

      auto length = static_cast<int>(rings.size());

      while (num < progressPos && num < length) {
        SetDestRotation(rings.get(num),
                        ringRotationEffect.RotationAngle + (static_cast<float>(num) * ringRotationEffect.RotationStep),
                        ringRotationEffect.RotationFlexySpeed);
        num++;
      }

      if (progressPos >= rings.size()) {
        RecycleRingRotationEffect(ringRotationEffect);
        _activeRingRotationEffects.erase(std::next(it).base());
      }
    }
  }
}

ChromaRotationEffect ChromaRingsRotationEffect::SpawnRingRotationEffect() {
  if (!_ringRotationEffectsPool.empty()) {
    auto first = _ringRotationEffectsPool.begin();
    ChromaRotationEffect result = *first;

    _ringRotationEffectsPool.erase(first);

    return result;
  }

  return {};
}

void ChromaRingsRotationEffect::RecycleRingRotationEffect(ChromaRotationEffect const& ringRotationEffect) {
  _ringRotationEffectsPool.emplace_back(ringRotationEffect);
}

float ChromaRingsRotationEffect::GetFirstRingRotationAngle() {
  return GetFirstRingRotationAngleCpp();
}

float ChromaRingsRotationEffect::GetFirstRingDestinationRotationAngle() {
  return GetFirstRingDestinationRotationAngleCpp();
}