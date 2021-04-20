#include "lighting/ChromaRingsRotationEffect.hpp"
#include "GlobalNamespace/TrackLaneRingsRotationEffect_RingRotationEffect.hpp"
#include "GlobalNamespace/TrackLaneRing.hpp"


using namespace Chroma;

DEFINE_TYPE(ChromaRingsRotationEffect);
DEFINE_TYPE(ChromaRotationEffect);

void ChromaRingsRotationEffect::ctor() {
    INVOKE_CTOR();
}

void ChromaRingsRotationEffect::AddRingRotationEffect(float angle, float step, int propagationSpeed, float flexySpeed) {
    AddRingRotationEffectF(angle, step, (float) propagationSpeed, flexySpeed);
}

void ChromaRingsRotationEffect::AddRingRotationEffectF(float angle, float step, float propagationSpeed,
                                                       float flexySpeed) {
    ChromaRotationEffect* ringRotationEffect = SpawnRingRotationEffect();
    ringRotationEffect->ProgressPos = 0;
    ringRotationEffect->RotationAngle = angle;
    ringRotationEffect->RotationStep = step;
    ringRotationEffect->RotationPropagationSpeed = propagationSpeed;
    ringRotationEffect->RotationFlexySpeed = flexySpeed;
    _activeRingRotationEffects.push_back(ringRotationEffect);
}

void ChromaRingsRotationEffect::SetNewRingManager(GlobalNamespace::TrackLaneRingsManager *trackLaneRingsManager) {
    _trackLaneRingsManager = trackLaneRingsManager;
}

void ChromaRingsRotationEffect::CopyValues(
        GlobalNamespace::TrackLaneRingsRotationEffect *trackLaneRingsRotationEffect) {
    _trackLaneRingsManager = trackLaneRingsRotationEffect->trackLaneRingsManager;
    _startupRotationAngle = trackLaneRingsRotationEffect->startupRotationAngle;
    _startupRotationStep = trackLaneRingsRotationEffect->startupRotationStep;
    _startupRotationPropagationSpeed = trackLaneRingsRotationEffect->startupRotationPropagationSpeed;
    _startupRotationFlexySpeed = trackLaneRingsRotationEffect->startupRotationFlexySpeed;
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
    if (!_activeRingRotationEffects.empty()) {
        auto rings = _trackLaneRingsManager->rings;

        // Reverse iterate so we can delete while iterating
        for (auto it = _activeRingRotationEffects.rbegin(); it != _activeRingRotationEffects.rend(); it++) {
            ChromaRotationEffect *ringRotationEffect = *it;
            int num = (int) ringRotationEffect->ProgressPos;
            auto progressPos = ringRotationEffect->ProgressPos += ringRotationEffect->RotationPropagationSpeed;

            int length = rings->Length();

            while ((float) num < progressPos && num < length) {
                rings->values[num]->SetDestRotation(
                        ringRotationEffect->RotationAngle + ((float) num * ringRotationEffect->RotationStep),
                        ringRotationEffect->RotationFlexySpeed);
                num++;
            }

            if ((int) progressPos >= rings->Length()) {
                RecycleRingRotationEffect(ringRotationEffect);
                _activeRingRotationEffects.erase(std::next(it).base());
            }
        }
    }
}

ChromaRotationEffect* ChromaRingsRotationEffect::SpawnRingRotationEffect() {
    ChromaRotationEffect* result;
    if (!_ringRotationEffectsPool.empty())
    {
        auto first = _ringRotationEffectsPool.begin();
        result = *first;
        _ringRotationEffectsPool.erase(first);
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

float ChromaRingsRotationEffect::GetFirstRingRotationAngle() {
    return _trackLaneRingsManager->rings->values[0]->GetRotation();
}

float ChromaRingsRotationEffect::GetFirstRingDestinationRotationAngle() {
    return _trackLaneRingsManager->rings->values[0]->GetDestinationRotation();
}