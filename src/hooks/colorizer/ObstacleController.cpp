#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "colorizer/ObstacleColorizer.hpp"
#include "GlobalNamespace/ColorManager.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/ObstacleController.hpp"
#include "GlobalNamespace/MultiplayerConnectedPlayerObstacleController.hpp"
#include "GlobalNamespace/VariableMovementDataProvider.hpp"
#include "utils/ChromaUtils.hpp"
#include "utils/VariableMovementHelper.hpp"

#include "ChromaObjectData.hpp"
#include "AnimationHelper.hpp"

#include "utils/ChromaAudioTimeSourceHelper.hpp"
#include <vector>

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace Chroma;
using namespace ChromaUtils;

MAKE_HOOK_MATCH(ObstacleController_Init, &ObstacleController::Init, void, ObstacleController* self,
                ::GlobalNamespace::ObstacleData* obstacleData,
                ::ByRef<::GlobalNamespace::ObstacleSpawnData> obstacleSpawnData) {
  static auto* MultiplayerConnectedPlayerObstacleControllerKlass =
      classof(MultiplayerConnectedPlayerObstacleController*);

  ObstacleController_Init(self, obstacleData, obstacleSpawnData);

  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks() ||
      ASSIGNMENT_CHECK(MultiplayerConnectedPlayerObstacleControllerKlass, self->klass)) {
    return;
  }

  auto const& chromaData =  getObjectAD(obstacleData);
  if (chromaData) {
    auto const& color = chromaData->Color;

    ObstacleColorizer::ColorizeObstacle(self, color);
  }
}
// Update is too small, use manual update yay
MAKE_HOOK_MATCH(ObstacleController_ManualUpdate, &ObstacleController::ManualUpdate, void, ObstacleController* self) {
  static auto* MultiplayerConnectedPlayerObstacleControllerKlass =
      classof(MultiplayerConnectedPlayerObstacleController*);

  ObstacleController_ManualUpdate(self);

  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks() ||
      ASSIGNMENT_CHECK(MultiplayerConnectedPlayerObstacleControllerKlass, self->klass)) {
    return;
  }

  auto const& chromaData =  getObjectAD(self->_obstacleData);
  if (chromaData) {
    auto const& color = chromaData->Color;

    auto const& tracks = chromaData->Tracks;

    auto const& pathPointDefinition = chromaData->LocalPathColor;
    if (!tracks.empty() || pathPointDefinition) {
      VariableMovementW movement(self->_variableMovementDataProvider);

      float jumpDuration = movement.jumpDuration;
      float elapsedTime =
          ChromaTimeSourceHelper::getSongTimeChroma(self->_audioTimeSyncController) - self->_startTimeOffset;
      float normalTime =
          (elapsedTime - movement.moveDuration) / (jumpDuration + self->_obstacleData->duration);

      [[maybe_unused]] bool updated = false;
      std::optional<Sombrero::FastColor> colorOffset =
          AnimationHelper::GetColorOffset(pathPointDefinition, tracks, normalTime, updated, TimeUnit());

      if (colorOffset) {
        ObstacleColorizer::ColorizeObstacle(self, colorOffset);
      }
    }
  }
}

void ObstacleControllerHook() {
  INSTALL_HOOK(ChromaLogger::Logger, ObstacleController_Init);
  INSTALL_HOOK(ChromaLogger::Logger, ObstacleController_ManualUpdate);
}

ChromaInstallHooks(ObstacleControllerHook)
