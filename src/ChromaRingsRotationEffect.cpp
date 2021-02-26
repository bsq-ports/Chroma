#include "ChromaRingsRotationEffect.hpp"
#include "GlobalNamespace/TrackLaneRingsRotationEffect_RingRotationEffect.hpp"
#include "GlobalNamespace/TrackLaneRing.hpp"


using namespace Chroma;

DEFINE_CLASS(ChromaRingsRotationEffect);
DEFINE_CLASS(ChromaRotationEffect);

void ChromaRingsRotationEffect::AddRingRotationEffect(float angle, float step, int propagationSpeed, float flexySpeed) {
    AddRingRotationEffectF(angle, step, (float) propagationSpeed, flexySpeed);
}

void ChromaRingsRotationEffect::AddRingRotationEffectF(float angle, float step, float propagationSpeed,
                                                       float flexySpeed) {
    getLogger().debug("Adding ring effect");
    ChromaRotationEffect* ringRotationEffect = ChromaRingsRotationEffect::SpawnRingRotationEffect();
    ringRotationEffect->ProgressPos = 0;
    ringRotationEffect->RotationAngle = angle;
    ringRotationEffect->RotationStep = step;
    ringRotationEffect->RotationPropagationSpeed = propagationSpeed;
    ringRotationEffect->RotationFlexySpeed = flexySpeed;
    _activeRingRotationEffects.push_back(ringRotationEffect);
}

void ChromaRingsRotationEffect::CopyValues(
        GlobalNamespace::TrackLaneRingsRotationEffect *trackLaneRingsRotationEffect) {
    getLogger().debug("Copying values");
    _trackLaneRingsManager = trackLaneRingsRotationEffect->trackLaneRingsManager;
    _startupRotationAngle = trackLaneRingsRotationEffect->startupRotationAngle;
    _startupRotationStep = trackLaneRingsRotationEffect->startupRotationStep;
    _startupRotationPropagationSpeed = trackLaneRingsRotationEffect->startupRotationPropagationSpeed;
    _startupRotationFlexySpeed = trackLaneRingsRotationEffect->startupRotationFlexySpeed;
    getLogger().debug("Copied values");
}

void ChromaRingsRotationEffect::Awake() {
    _activeRingRotationEffects = std::vector<ChromaRotationEffect*>();
    _activeRingRotationEffects.reserve(20);
    _ringRotationEffectsPool = std::vector<ChromaRotationEffect*>();
    _ringRotationEffectsPool.reserve(20);
    for (int i = 0; i < _ringRotationEffectsPool.capacity(); i++)
    {
        _ringRotationEffectsPool.push_back(CRASH_UNLESS(il2cpp_utils::New<ChromaRotationEffect*>()));
    }
}

void ChromaRingsRotationEffect::Start() {
    AddRingRotationEffect(_startupRotationAngle, _startupRotationStep, _startupRotationPropagationSpeed, _startupRotationFlexySpeed);
}

void ChromaRingsRotationEffect::FixedUpdate() {
    auto rings = _trackLaneRingsManager->rings;

//    getLogger().debug("Doing ring stuff %d pool %d", _activeRingRotationEffects.size(), _ringRotationEffectsPool.size());

    // Use this vector avoid deleting to the original vector while iterating through it
    std::vector<std::vector<ChromaRotationEffect *>::iterator> iteratorsToDelete;

    auto random = std::chrono::system_clock::now();
    for (auto it = _activeRingRotationEffects.begin(); it != _activeRingRotationEffects.end(); it++) {
        ChromaRotationEffect *ringRotationEffect = *it;
        int num = (int) ringRotationEffect->ProgressPos;
        ringRotationEffect->ProgressPos += ringRotationEffect->RotationPropagationSpeed;
        while ((float) num < ringRotationEffect->ProgressPos && num < rings->Length()) {
            getLogger().debug("While loop %d", random);
            rings->values[num]->SetDestRotation(
                    ringRotationEffect->RotationAngle + ((float) num * ringRotationEffect->RotationStep),
                    ringRotationEffect->RotationFlexySpeed);
            num++;
        }

        if ((int) ringRotationEffect->ProgressPos >= rings->Length()) {
            getLogger().debug("While loop %d", random);
            RecycleRingRotationEffect(ringRotationEffect);
            iteratorsToDelete.push_back(it);
        }
    }

    if (!iteratorsToDelete.empty())
        for (auto &it : iteratorsToDelete) {
            _activeRingRotationEffects.erase(it);
        }
}

ChromaRotationEffect* ChromaRingsRotationEffect::SpawnRingRotationEffect() {
    ChromaRotationEffect* result;
    if (!_ringRotationEffectsPool.empty())
    {
        result = _ringRotationEffectsPool[0];
        _ringRotationEffectsPool.erase(_ringRotationEffectsPool.begin());
    }
    else
    {
        result = CRASH_UNLESS(il2cpp_utils::New<ChromaRotationEffect*>());
    }

    return result;
}

void ChromaRingsRotationEffect::RecycleRingRotationEffect(ChromaRotationEffect* ringRotationEffect) {
    _ringRotationEffectsPool.push_back(ringRotationEffect);
}