#include "main.hpp"
#include "Chroma.hpp"
#include "ChromaController.hpp"
#include "ChromaLogger.hpp"

#include "utils/ChromaUtils.hpp"

#include "hooks/TrackLaneRingsManager.hpp"
#include "environment_enhancements/EnvironmentEnhancementManager.hpp"

#include "GlobalNamespace/TrackLaneRing.hpp"

#include "UnityEngine/Quaternion.hpp"

using namespace Chroma;
using namespace ChromaUtils;
using namespace GlobalNamespace;

MAKE_HOOK_MATCH(TrackLaneRing_Init, &TrackLaneRing::Init, void, GlobalNamespace::TrackLaneRing* self,
                UnityEngine::Vector3 position, UnityEngine::Vector3 positionOffset) {
  TrackLaneRing_Init(self, position, positionOffset);
  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    return;
  }

  self->_posZ = position.z;
}

MAKE_HOOK_MATCH(TrackLaneRing_FixedUpdateRing, &TrackLaneRing::FixedUpdateRing, void,
                GlobalNamespace::TrackLaneRing* self, float fixedDeltaTime) {
  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    TrackLaneRing_FixedUpdateRing(self, fixedDeltaTime);
    return;
  }

  self->_prevRotZ = self->_rotZ;
  self->_rotZ = std::lerp(self->_rotZ, self->_destRotZ, Sombrero::Clamp01(fixedDeltaTime * self->_rotationSpeed));
  self->_prevPosZ = self->_posZ;
  self->_posZ = std::lerp(self->_posZ, self->_destPosZ, Sombrero::Clamp01(fixedDeltaTime * self->_moveSpeed));
}

MAKE_HOOK_MATCH(TrackLaneRing_LateUpdateRing, &TrackLaneRing::LateUpdateRing, void,
                GlobalNamespace::TrackLaneRing* self, float interpolationFactor) {
  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    TrackLaneRing_LateUpdateRing(self, interpolationFactor);
    return;
  }

  Sombrero::FastQuaternion rotation = Sombrero::FastQuaternion::identity();

  auto it2 = EnvironmentEnhancementManager::RingRotationOffsets.find(self);

  if (it2 != EnvironmentEnhancementManager::RingRotationOffsets.end()) {
    rotation = it2->second;
  }

  float interpolatedZPos = self->_prevPosZ + ((self->_posZ - self->_prevPosZ) * interpolationFactor);
  Sombrero::FastVector3 positionZOffset = (rotation * Sombrero::FastVector3::forward()) * interpolatedZPos;
  Sombrero::FastVector3 pos = Sombrero::FastVector3(self->_positionOffset) + positionZOffset;

  float interpolatedZRot = self->_prevRotZ + ((self->_rotZ - self->_prevRotZ) * interpolationFactor);

  static auto AngleAxis = FPtrWrapper<&Sombrero::FastQuaternion::AngleAxis>::get();

  Sombrero::FastQuaternion rotationZOffset = AngleAxis(interpolatedZRot, Sombrero::FastVector3::forward());
  Sombrero::FastQuaternion rot = rotation * rotationZOffset;

  self->transform->set_localRotation(rot);
  self->transform->set_localPosition(pos);
}

void TrackLaneRingHook() {
  INSTALL_HOOK(ChromaLogger::Logger, TrackLaneRing_Init);
  INSTALL_HOOK(ChromaLogger::Logger, TrackLaneRing_FixedUpdateRing);
  INSTALL_HOOK(ChromaLogger::Logger, TrackLaneRing_LateUpdateRing);
}

ChromaInstallHooks(TrackLaneRingHook)