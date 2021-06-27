#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "colorizer/ObstacleColorizer.hpp"
#include "GlobalNamespace/ColorManager.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/ObstacleController.hpp"
#include "GlobalNamespace/MultiplayerConnectedPlayerObstacleController.hpp"
#include "utils/ChromaUtils.hpp"

#include "ChromaObjectData.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace Chroma;
using namespace ChromaUtils;

MAKE_HOOK_OFFSETLESS(
    ObstacleController_Init,
    void,
    ObstacleController* self,
    ObstacleData* obstacleData,
    float worldRotation,
    UnityEngine::Vector3 startPos,
    UnityEngine::Vector3 midPos,
    UnityEngine::Vector3 endPos,
    float move1Duration,
    float move2Duration,
    float singleLineWidth,
    float height
) {
    static auto MultiplayerConnectedPlayerObstacleControllerKlass = classof(MultiplayerConnectedPlayerObstacleController*);

    ObstacleController_Init(self, obstacleData, worldRotation, startPos, midPos, endPos, move1Duration, move2Duration, singleLineWidth, height);

    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks() || ASSIGNMENT_CHECK(MultiplayerConnectedPlayerObstacleControllerKlass, self->klass)) {
        return;
    }


    auto chromaData = ChromaObjectDataManager::ChromaObjectDatas.find(obstacleData);
    if (chromaData != ChromaObjectDataManager::ChromaObjectDatas.end()) {
        auto color = chromaData->second->Color;

        ObstacleColorizer::ColorizeObstacle(self, color);
    }
}

// TODO: Heck Update

void Chroma::Hooks::ObstacleController() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), ObstacleController_Init, il2cpp_utils::FindMethodUnsafe("", "ObstacleController", "Init", 9));
}