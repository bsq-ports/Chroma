#include "main.hpp"
#include "Chroma.hpp"
#include "ChromaController.hpp"
#include "utils/ChromaUtils.hpp"

#include "hooks/TrackLaneRingsManager.hpp"
#include "lighting/environment_enhancements/EnvironmentEnhancementManager.hpp"

#include "GlobalNamespace/TrackLaneRing.hpp"

#include "UnityEngine/Quaternion.hpp"

using namespace Chroma;
using namespace ChromaUtils;
using namespace GlobalNamespace;

MAKE_HOOK_MATCH(TrackLaneRing_Init, &TrackLaneRing::Init, void, GlobalNamespace::TrackLaneRing* self, UnityEngine::Vector3 position, UnityEngine::Vector3 positionOffset) {
    TrackLaneRing_Init(self, position, positionOffset);
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        return;
    }

    self->posZ = position.z;
}

MAKE_HOOK_MATCH(TrackLaneRing_FixedUpdateRing, &TrackLaneRing::FixedUpdateRing, void, GlobalNamespace::TrackLaneRing* self, float fixedDeltaTime) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        TrackLaneRing_FixedUpdateRing(self, fixedDeltaTime);
        return;
    }

    self->prevRotZ = self->rotZ;
    self->rotZ = Lerp(self->rotZ, self->destRotZ, fixedDeltaTime * self->rotationSpeed);
    self->prevPosZ = self->posZ;
    self->posZ = Lerp(self->posZ, self->destPosZ, fixedDeltaTime * self->moveSpeed);
}

MAKE_HOOK_MATCH(TrackLaneRing_LateUpdateRing, &TrackLaneRing::LateUpdateRing, void, GlobalNamespace::TrackLaneRing* self, float interpolationFactor) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        TrackLaneRing_LateUpdateRing(self, interpolationFactor);
        return;
    }

    static UnityEngine::Quaternion identity = UnityEngine::Quaternion::get_identity();

    auto& rotation = identity;

    auto it2 = EnvironmentEnhancementManager::RingRotationOffsets.find(self);

    if (it2 != EnvironmentEnhancementManager::RingRotationOffsets.end()) {
        rotation = it2->second;
    }


    static UnityEngine::Vector3 vectorForward = UnityEngine::Vector3::get_forward();

    float interpolatedZPos = self->prevPosZ + ((self->posZ - self->prevPosZ) * interpolationFactor);
    UnityEngine::Vector3 positionZOffset = vectorMultiply(quaternionMultiply(rotation, vectorForward), interpolatedZPos);
    UnityEngine::Vector3 pos = vectorAdd(self->positionOffset, positionZOffset);

    float interpolatedZRot = self->prevRotZ + ((self->rotZ - self->prevRotZ) * interpolationFactor);

    MOD_PTR_CACHE(&UnityEngine::Quaternion::AngleAxis, angleAxis, UnityEngine::Quaternion, float, UnityEngine::Vector3)

    auto rotationZOffset = angleAxis(interpolatedZRot, vectorForward);
    UnityEngine::Quaternion rot = quaternionMultiply(rotation, rotationZOffset);

    self->transform->set_localRotation(rot);
    self->transform->set_localPosition(pos);
}

void TrackLaneRingHook(Logger& logger) {
    INSTALL_HOOK(logger, TrackLaneRing_Init);
    INSTALL_HOOK(logger, TrackLaneRing_FixedUpdateRing);
    INSTALL_HOOK(logger, TrackLaneRing_LateUpdateRing);
    //    INSTALL_HOOK_OFFSETLESS(getLogger(), SaberManager_Finalize, il2cpp_utils::FindMethodUnsafe("System", "Object", "Finalize", 0));
}

ChromaInstallHooks(TrackLaneRingHook)