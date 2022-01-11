#include "Chroma.hpp"
#include "lighting/ChromaRingsRotationEffect.hpp"
#include "GlobalNamespace/TrackLaneRingsRotationEffect_RingRotationEffect.hpp"
#include "GlobalNamespace/TrackLaneRing.hpp"

using namespace Chroma;

DEFINE_TYPE(Chroma, ChromaRingsRotationEffect);

void ChromaRingsRotationEffect::AddRingRotationEffect(float angle, float step, int propagationSpeed, float flexySpeed) {
    AddRingRotationEffectF(angle, step, (float) propagationSpeed, flexySpeed);
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

void ChromaRingsRotationEffect::SetNewRingManager(GlobalNamespace::TrackLaneRingsManager *trackLaneRingsManager) {
    this->trackLaneRingsManager = trackLaneRingsManager;
}

void ChromaRingsRotationEffect::CopyValues(
        GlobalNamespace::TrackLaneRingsRotationEffect *trackLaneRingsRotationEffect) {
    this->trackLaneRingsManager = trackLaneRingsRotationEffect->trackLaneRingsManager;
    this->startupRotationAngle = trackLaneRingsRotationEffect->startupRotationAngle;
    this->startupRotationStep = trackLaneRingsRotationEffect->startupRotationStep;
    this->startupRotationPropagationSpeed = trackLaneRingsRotationEffect->startupRotationPropagationSpeed;
    this->startupRotationFlexySpeed = trackLaneRingsRotationEffect->startupRotationFlexySpeed;
}

void ChromaRingsRotationEffect::Awake() {
    int poolCount = 20;
    _activeRingRotationEffects = std::vector<ChromaRotationEffect>();
    _activeRingRotationEffects.reserve(poolCount);
    _ringRotationEffectsPool = std::vector<ChromaRotationEffect>(poolCount);
}

void ChromaRingsRotationEffect::Start() {
    AddRingRotationEffectF(startupRotationAngle, startupRotationStep, (float) startupRotationPropagationSpeed, startupRotationFlexySpeed);
}

void ChromaRingsRotationEffect::FixedUpdate() {
    if (!_activeRingRotationEffects.empty()) {
        auto rings = trackLaneRingsManager->rings;

        if (!rings) {
            getLogger().warning("Rings is null why! %p ", trackLaneRingsManager);
            rings = trackLaneRingsManager->rings = Array<GlobalNamespace::TrackLaneRing *>::New();
        }

        static auto SetDestRotation = FPtrWrapper<&GlobalNamespace::TrackLaneRing::SetDestRotation>::get();

        // Reverse iterate so we can delete while iterating
        for (auto it = _activeRingRotationEffects.rbegin(); it != _activeRingRotationEffects.rend(); it++) {
            ChromaRotationEffect& ringRotationEffect = *it;
            auto num = (long) ringRotationEffect.ProgressPos;
            auto progressPos = ringRotationEffect.ProgressPos += ringRotationEffect.RotationPropagationSpeed;

            auto length = (int) rings.Length();

            while (num < progressPos && num < length) {
                SetDestRotation(rings.get(num),
                        ringRotationEffect.RotationAngle + ((float) num * ringRotationEffect.RotationStep),
                        ringRotationEffect.RotationFlexySpeed);
                num++;
            }

            if (progressPos >= rings.Length()) {
                RecycleRingRotationEffect(ringRotationEffect);
                _activeRingRotationEffects.erase(std::next(it).base());
            }
        }
    }
}

ChromaRotationEffect ChromaRingsRotationEffect::SpawnRingRotationEffect() {
    if (!_ringRotationEffectsPool.empty())
    {
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