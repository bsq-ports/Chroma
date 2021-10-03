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
    self->rotZ = Sombrero::Lerp(self->rotZ, self->destRotZ, fixedDeltaTime * self->rotationSpeed);
    self->prevPosZ = self->posZ;
    self->posZ = Sombrero::Lerp(self->posZ, self->destPosZ, fixedDeltaTime * self->moveSpeed);
}

MAKE_HOOK_MATCH(TrackLaneRing_LateUpdateRing, &TrackLaneRing::LateUpdateRing, void, GlobalNamespace::TrackLaneRing* self, float interpolationFactor) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        TrackLaneRing_LateUpdateRing(self, interpolationFactor);
        return;
    }

    const static Sombrero::FastQuaternion identity = Sombrero::FastQuaternion::identity();

    Sombrero::FastQuaternion rotation = identity;

    auto it2 = EnvironmentEnhancementManager::RingRotationOffsets.find(self);

    if (it2 != EnvironmentEnhancementManager::RingRotationOffsets.end()) {
        rotation = it2->second;
    }


    const static Sombrero::FastVector3 vectorForward = Sombrero::FastVector3::forward();

    float interpolatedZPos = self->prevPosZ + ((self->posZ - self->prevPosZ) * interpolationFactor);
    Sombrero::FastVector3 positionZOffset = ((rotation * vectorForward) * interpolatedZPos);
    Sombrero::FastVector3 pos = Sombrero::vector3add(self->positionOffset, positionZOffset);

    float interpolatedZRot = self->prevRotZ + ((self->rotZ - self->prevRotZ) * interpolationFactor);

    static auto AngleAxis = FPtrWrapper<&Sombrero::FastQuaternion::AngleAxis>::get();

    Sombrero::FastQuaternion rotationZOffset = AngleAxis(interpolatedZRot, vectorForward);
    Sombrero::FastQuaternion rot = rotation * rotationZOffset;

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