#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "colorizer/ObstacleColorizer.hpp"
#include "GlobalNamespace/ColorManager.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/ObstacleController.hpp"
#include "GlobalNamespace/MultiplayerConnectedPlayerObstacleController.hpp"
#include "utils/ChromaUtils.hpp"

#include "ChromaObjectData.hpp"
#include "AnimationHelper.hpp"

#include "utils/ChromaAudioTimeSourceHelper.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace Chroma;
using namespace ChromaUtils;

MAKE_HOOK_MATCH(
        ObstacleController_Init,
        &ObstacleController::Init,
        void,
        ObstacleController *self,
        ObstacleData *obstacleData,
        float worldRotation,
        UnityEngine::Vector3 startPos,
        UnityEngine::Vector3 midPos,
        UnityEngine::Vector3 endPos,
        float move1Duration,
        float move2Duration,
        float singleLineWidth,
        float height
) {
    static auto MultiplayerConnectedPlayerObstacleControllerKlass = classof(
            MultiplayerConnectedPlayerObstacleController*);

    ObstacleController_Init(self, obstacleData, worldRotation, startPos, midPos, endPos, move1Duration, move2Duration,
                            singleLineWidth, height);

    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks() ||
        ASSIGNMENT_CHECK(MultiplayerConnectedPlayerObstacleControllerKlass, self->klass)) {
        return;
    }


    auto chromaData = ChromaObjectDataManager::ChromaObjectDatas.find(obstacleData);
    if (chromaData != ChromaObjectDataManager::ChromaObjectDatas.end()) {
        auto const &color = chromaData->second->Color;

        ObstacleColorizer::ColorizeObstacle(self, color);
    }
}
// Update is too small, use manual update yay
MAKE_HOOK_MATCH(
        ObstacleController_ManualUpdate,
        &ObstacleController::ManualUpdate,
        void,
        ObstacleController * self
) {
    static auto MultiplayerConnectedPlayerObstacleControllerKlass = classof(
            MultiplayerConnectedPlayerObstacleController*);

    ObstacleController_ManualUpdate(self);

    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks() ||
        ASSIGNMENT_CHECK(MultiplayerConnectedPlayerObstacleControllerKlass, self->klass)) {
        return;
    }


    auto chromaData = ChromaObjectDataManager::ChromaObjectDatas.find(self->obstacleData);
    if (chromaData != ChromaObjectDataManager::ChromaObjectDatas.end()) {
        auto const &tracks = chromaData->second->Tracks;
        auto const &pathPointDefinition = chromaData->second->LocalPathColor;
        if (!tracks.empty() || pathPointDefinition) {
            float jumpDuration = self->move2Duration;
            float elapsedTime =
                    ChromaTimeSourceHelper::getSongTimeChroma(self->audioTimeSyncController) - self->startTimeOffset;
            float normalTime = (elapsedTime - self->move1Duration) / (jumpDuration + self->obstacleDuration);

            std::optional<Sombrero::FastColor> colorOffset;
            AnimationHelper::GetColorOffset(pathPointDefinition, tracks, normalTime, colorOffset);

            if (colorOffset) {
                ObstacleColorizer::ColorizeObstacle(self, colorOffset.value());
            }
        }
    }
}

void ObstacleControllerHook(Logger &logger) {
    INSTALL_HOOK(getLogger(), ObstacleController_Init);
    INSTALL_HOOK(getLogger(), ObstacleController_ManualUpdate);
}

ChromaInstallHooks(ObstacleControllerHook)