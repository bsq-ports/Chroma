#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "colorizer/ObstacleColorizer.hpp"
#include "GlobalNamespace/ColorManager.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/ObstacleController.hpp"
#include "GlobalNamespace/MultiplayerConnectedPlayerObstacleController.hpp"
#include "GlobalNamespace/VariableMovementDataProvider.hpp"
#include "utils/ChromaUtils.hpp"

#include "ChromaObjectData.hpp"
#include "AnimationHelper.hpp"

#include "utils/ChromaAudioTimeSourceHelper.hpp"

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

  auto chromaData = ChromaObjectDataManager::ChromaObjectDatas.find(obstacleData);
  if (chromaData != ChromaObjectDataManager::ChromaObjectDatas.end()) {
    auto const& color = chromaData->second.Color;

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

  auto chromaData = ChromaObjectDataManager::ChromaObjectDatas.find(self->obstacleData);
  if (chromaData != ChromaObjectDataManager::ChromaObjectDatas.end()) {
    auto const& tracks = chromaData->second.Tracks;
    auto const& pathPointDefinition = chromaData->second.LocalPathColor;
    if (!tracks.empty() || pathPointDefinition) {
      float jumpDuration = self->_variableMovementDataProvider->jumpDuration;
      float elapsedTime =
          ChromaTimeSourceHelper::getSongTimeChroma(self->_audioTimeSyncController) - self->_startTimeOffset;
      float normalTime =
          (elapsedTime - self->_variableMovementDataProvider->moveDuration) / (jumpDuration + self->_obstacleDuration);

      [[maybe_unused]] bool updated = false;
      std::optional<Sombrero::FastColor> colorOffset =
          AnimationHelper::GetColorOffset(pathPointDefinition, tracks, normalTime, updated, 0);

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