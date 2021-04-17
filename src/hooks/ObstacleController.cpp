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
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks() || ASSIGNMENT_CHECK(classof(MultiplayerConnectedPlayerObstacleController*), self->klass)) {
        ObstacleController_Init(self, obstacleData, worldRotation, startPos, midPos, endPos, move1Duration, move2Duration, singleLineWidth, height);
        return;
    }
    ObstacleColorizer::OCStart(self, self->colorManager->get_obstaclesColor());

    auto chromaData = (ChromaObjectDataManager::ChromaObjectDatas[obstacleData]);
    auto color = chromaData->Color;



    if (color) {
        ObstacleColorizer::SetObstacleColor(self, color.value());
    } else {
        ObstacleColorizer::Reset(self);
    }


    ObstacleController_Init(self, obstacleData, worldRotation, startPos, midPos, endPos, move1Duration, move2Duration, singleLineWidth, height);

    ObstacleColorizer::SetActiveColors(self);
}

void Chroma::Hooks::ObstacleController() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), ObstacleController_Init, il2cpp_utils::FindMethodUnsafe("", "ObstacleController", "Init", 9));
}