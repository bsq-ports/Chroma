#include "Chroma.hpp"
#include "lighting/ChromaRingsRotationEffect.hpp"
#include "GlobalNamespace/TrackLaneRingsRotationEffect_RingRotationEffect.hpp"
#include "GlobalNamespace/TrackLaneRing.hpp"


using namespace Chroma;

DEFINE_TYPE(Chroma, ChromaRingsRotationEffect);
DEFINE_TYPE(Chroma, ChromaRotationEffect);

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
    _activeRingRotationEffects = std::vector<ChromaRotationEffect*>();
    _activeRingRotationEffects.reserve(poolCount);
    _ringRotationEffectsPool = std::vector<ChromaRotationEffect*>();
    _ringRotationEffectsPool.reserve(poolCount);
#pragma unroll 20
    for (int i = 0; i < poolCount; i++)
    {
        _ringRotationEffectsPool.push_back(CRASH_UNLESS(il2cpp_utils::New<ChromaRotationEffect*>()));
    }
}

void ChromaRingsRotationEffect::Start() {
    AddRingRotationEffectF(startupRotationAngle, startupRotationStep, (float) startupRotationPropagationSpeed, startupRotationFlexySpeed);
}

void ChromaRingsRotationEffect::FixedUpdate() {
    if (!_activeRingRotationEffects.empty()) {
        auto rings = trackLaneRingsManager->rings;

        if (!rings) {
             // TODO: How to fix?
            getLogger().warning("Rings is null why! %p ", trackLaneRingsManager);
            rings = trackLaneRingsManager->rings = Array<GlobalNamespace::TrackLaneRing *>::New();
        }

        static auto SetDestRotation = FPtrWrapper<&GlobalNamespace::TrackLaneRing::SetDestRotation>::get();

        // Reverse iterate so we can delete while iterating
        for (auto it = _activeRingRotationEffects.rbegin(); it != _activeRingRotationEffects.rend(); it++) {
            ChromaRotationEffect *ringRotationEffect = *it;
            long num = (long) ringRotationEffect->ProgressPos;
            auto progressPos = ringRotationEffect->ProgressPos += ringRotationEffect->RotationPropagationSpeed;

            int length = (int) rings->Length();

            while (num < progressPos && num < length) {
                SetDestRotation(rings->get(num),
                        ringRotationEffect->RotationAngle + ((float) num * ringRotationEffect->RotationStep),
                        ringRotationEffect->RotationFlexySpeed);
                num++;
            }

            if (progressPos >= rings->Length()) {
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
    static auto GetRotation = FPtrWrapper<&GlobalNamespace::TrackLaneRing::GetRotation>::get();
    if (!trackLaneRingsManager->rings) {
        getLogger().warning("Rings is null why! %p ", trackLaneRingsManager);
        trackLaneRingsManager->rings = Array<GlobalNamespace::TrackLaneRing *>::New();
    }

    return GetRotation(trackLaneRingsManager->rings->get(0));
}

float ChromaRingsRotationEffect::GetFirstRingRotationAngleCpp() {
    static auto GetRotation = FPtrWrapper<&GlobalNamespace::TrackLaneRing::GetRotation>::get();
    if (!trackLaneRingsManager->rings) {
        getLogger().warning("Rings is null why! %p ", trackLaneRingsManager);
        trackLaneRingsManager->rings = Array<GlobalNamespace::TrackLaneRing *>::New();
    }

    return GetRotation(trackLaneRingsManager->rings->get(0));
}

float ChromaRingsRotationEffect::GetFirstRingDestinationRotationAngle() {
    static auto GetDestinationRotation = FPtrWrapper<&GlobalNamespace::TrackLaneRing::GetDestinationRotation>::get();

    if (!trackLaneRingsManager->rings) {
        getLogger().warning("Rings is null why! %p ", trackLaneRingsManager);
        trackLaneRingsManager->rings = Array<GlobalNamespace::TrackLaneRing *>::New();
    }

    return GetDestinationRotation(trackLaneRingsManager->rings->get(0));
}

float ChromaRingsRotationEffect::GetFirstRingDestinationRotationAngleCpp() {
    static auto GetDestinationRotation = FPtrWrapper<&GlobalNamespace::TrackLaneRing::GetDestinationRotation>::get();

    if (!trackLaneRingsManager->rings) {
        getLogger().warning("Rings is null why! %p ", trackLaneRingsManager);
        trackLaneRingsManager->rings = Array<GlobalNamespace::TrackLaneRing *>::New();
    }

    return GetDestinationRotation(trackLaneRingsManager->rings->get(0));
}