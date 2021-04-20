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

MAKE_HOOK_OFFSETLESS(TrackLaneRing_Init, void, GlobalNamespace::TrackLaneRing* self, UnityEngine::Vector3 position, UnityEngine::Vector3 positionOffset) {
    TrackLaneRing_Init(self, position, positionOffset);
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        return;
    }

    self->posZ = position.z;
}

MAKE_HOOK_OFFSETLESS(TrackLaneRing_FixedUpdateRing, void, GlobalNamespace::TrackLaneRing* self, float fixedDeltaTime) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        TrackLaneRing_FixedUpdateRing(self, fixedDeltaTime);
        return;
    }

    if (!EnvironmentEnhancementManager::SkipRingUpdate.empty())
    {
        auto it = EnvironmentEnhancementManager::SkipRingUpdate.find(self);

        if (it != EnvironmentEnhancementManager::SkipRingUpdate.end()) {
            auto doSkip = it->second;

            if (doSkip) {
                return;
            }
        }
    }

    self->prevRotZ = self->rotZ;
    self->rotZ = Lerp(self->rotZ, self->destRotZ, fixedDeltaTime * self->rotationSpeed);
    self->prevPosZ = self->posZ;
    self->posZ = Lerp(self->posZ, self->destPosZ, fixedDeltaTime * self->moveSpeed);
}

MAKE_HOOK_OFFSETLESS(TrackLaneRing_LateUpdateRing, void, GlobalNamespace::TrackLaneRing* self, float interpolationFactor) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        TrackLaneRing_LateUpdateRing(self, interpolationFactor);
        return;
    }

    if (!EnvironmentEnhancementManager::SkipRingUpdate.empty())
    {
        auto it = EnvironmentEnhancementManager::SkipRingUpdate.find(self);

        if (it != EnvironmentEnhancementManager::SkipRingUpdate.end()) {
            auto doSkip = it->second;

            if (doSkip) {
                return;
            }
        }

    }

    UnityEngine::Quaternion rotation = UnityEngine::Quaternion::get_identity();

    auto it2 = EnvironmentEnhancementManager::RingRotationOffsets.find(self);

    if (it2 != EnvironmentEnhancementManager::RingRotationOffsets.end())
    {
        rotation = UnityEngine::Quaternion::Euler(it2->second);
    }

        float interpolatedZPos = self->prevPosZ + ((self->posZ - self->prevPosZ) * interpolationFactor);
    UnityEngine::Vector3 positionZOffset = rotation * UnityEngine::Vector3::get_forward() * interpolatedZPos;
    UnityEngine::Vector3 pos = self->positionOffset + positionZOffset;

    float interpolatedZRot = self->prevRotZ + ((self->rotZ - self->prevRotZ) * interpolationFactor);
    auto rotationZOffset = UnityEngine::Quaternion::AngleAxis(interpolatedZRot, UnityEngine::Vector3::get_forward());
    UnityEngine::Quaternion rot = rotation * rotationZOffset;

    self->transform->set_localRotation(rot);
    self->transform->set_localPosition(pos);
}

void Hooks::TrackLaneRing() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), TrackLaneRing_FixedUpdateRing, il2cpp_utils::FindMethodUnsafe("", "TrackLaneRing", "FixedUpdateRing", 1));
    INSTALL_HOOK_OFFSETLESS(getLogger(), TrackLaneRing_LateUpdateRing, il2cpp_utils::FindMethodUnsafe("", "TrackLaneRing", "LateUpdateRing", 1));
    //    INSTALL_HOOK_OFFSETLESS(getLogger(), SaberManager_Finalize, il2cpp_utils::FindMethodUnsafe("System", "Object", "Finalize", 0));
}