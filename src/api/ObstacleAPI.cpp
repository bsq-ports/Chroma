#include "ObstacleAPI.hpp"
#include "colorizer/ObstacleColorizer.hpp"

#include "conditional-dependencies/shared/main.hpp"
#include "shared/utils.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;


// TODO: unsure of this

EXPOSE_API(getObstacleControllerColorSafe, int, ObstacleController* obstacleController) {

    auto ocm = OCColorManager::GetOCColorManager(obstacleController);

    if (!ocm) return -1;

    auto color = ocm->_color->color;

    int rgba = Chroma::ColourManager::ColourToInt(color);

    return rgba;
}

EXPOSE_API(setObstacleColorSafe, void, ObstacleController* oc, std::optional<UnityEngine::Color> color1) {
    ObstacleColorizer::SetObstacleColor(oc, color1);
}

EXPOSE_API(setAllObstacleColorSafe, void, std::optional<UnityEngine::Color> color1) {
    ObstacleColorizer::SetAllObstacleColors(color1);
}


