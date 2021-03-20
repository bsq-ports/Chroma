#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "colorizer/ObstacleColorizer.hpp"
#include "GlobalNamespace/ColorManager.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/ObstacleController.hpp"
#include "utils/ChromaUtils.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace Chroma;
using namespace ChromaUtils;

MAKE_HOOK_OFFSETLESS(
    ObstacleController_Init,
    void,
    ObstacleController* self,
    CustomObstacleData* obstacleData,
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
    if (!ChromaController::DoChromaHooks()) {
        ObstacleController_Init(self, obstacleData, worldRotation, startPos, midPos, endPos, move1Duration, move2Duration, singleLineWidth, height);
        return;
    }
    ObstacleColorizer::OCStart(self, self->colorManager->get_obstaclesColor());

    if(obstacleData->customData && obstacleData->customData->value) {
        std::optional<UnityEngine::Color> color = ChromaUtilities::GetColorFromData(obstacleData->customData->value);

        if (color) {
            ObstacleColorizer::SetObstacleColor(self, color);
        } else {
            ObstacleColorizer::Reset(self);
        }
    }

    ObstacleController_Init(self, obstacleData, worldRotation, startPos, midPos, endPos, move1Duration, move2Duration, singleLineWidth, height);

    ObstacleColorizer::SetActiveColors(self);
}

void Chroma::Hooks::ObstacleController() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), ObstacleController_Init, il2cpp_utils::FindMethodUnsafe("", "ObstacleController", "Init", 9));
}