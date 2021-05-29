#include "ObstacleAPI.hpp"
#include "colorizer/ObstacleColorizer.hpp"

#include "conditional-dependencies/shared/main.hpp"
#include "shared/utils.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;


// TODO: unsure of this

EXPOSE_API(getObstacleControllerColorSafe, OptColor, ObstacleController* obstacleController) {

    auto ocm = OCColorManager::GetOCColorManager(obstacleController);

    if (!ocm) return OptColor();

    auto color = ocm->_color;

    if (!color)
        return OptColor();

    return OptColorFromColor(color.value());
}

EXPOSE_API(setObstacleColorSafe, void, ObstacleController* oc, UnityEngine::Color color1) {
    ObstacleColorizer::SetObstacleColor(oc, color1);
}

EXPOSE_API(setAllObstacleColorSafe, void, UnityEngine::Color color1) {
    ObstacleColorizer::SetAllObstacleColors(color1);
}


