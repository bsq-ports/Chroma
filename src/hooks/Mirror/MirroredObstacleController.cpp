#include "Chroma.hpp"

#include "ChromaController.hpp"

#include "GlobalNamespace/BaseNoteVisuals.hpp"
#include "GlobalNamespace/BombNoteController.hpp"
#include "GlobalNamespace/MirroredBombNoteController.hpp"
#include "GlobalNamespace/MirroredObstacleController.hpp"
#include "GlobalNamespace/MultiplayerConnectedPlayerNoteController.hpp"
#include "GlobalNamespace/TutorialNoteController.hpp"
#include "GlobalNamespace/SaberBurnMarkArea.hpp"

#include "UnityEngine/ParticleSystem.hpp"

#include "colorizer/ObstacleColorizer.hpp"
#include "utils/ChromaUtils.hpp"

using namespace GlobalNamespace;
using namespace Chroma;
using namespace UnityEngine;

MAKE_HOOK_MATCH(MirroredObstacleController_UpdatePositionAndRotation,
                &MirroredObstacleController::UpdatePositionAndRotation, void, MirroredObstacleController* self) {
  MirroredObstacleController_UpdatePositionAndRotation(self);

  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    return;
  }

  if (self->_followedObstacle == nullptr) {
    return;
  }

  ObstacleColorizer::ColorizeObstacle(self,
                                      ObstacleColorizer::GetObstacleColorizer(self->_followedObstacle)->getColor());
}

void MirroredObstacleControllerHook(Logger& logger) {
  INSTALL_HOOK(logger, MirroredObstacleController_UpdatePositionAndRotation);
}

ChromaInstallHooks(MirroredObstacleControllerHook)