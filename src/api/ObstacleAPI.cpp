#include "ObstacleAPI.hpp"
#include "colorizer/ObstacleColorizer.hpp"

#include "conditional-dependencies/shared/main.hpp"
#include "shared/utils.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Sombrero;


// TODO: unsure of this

EXPOSE_API(getObstacleControllerColorSafe, OptColor, ObstacleController* obstacleController) {

    auto ocm = ObstacleColorizer::GetObstacleColorizer(obstacleController);

    if (!ocm) return OptColor();

    auto color = ocm->getColor();


    return OptColorFromColor(color);
}

EXPOSE_API(setObstacleColorSafe, void, ObstacleController* oc, Sombrero::FastColor color1) {
    ObstacleColorizer::ColorizeObstacle(oc, color1);
}

EXPOSE_API(setAllObstacleColorSafe, void, Sombrero::FastColor color1) {
    ObstacleColorizer::GlobalColorize(color1);
}

extern "C" void __setObstacleColorable(bool colorable) {
    ObstacleColorizer::ObstacleColorable = colorable;
}

extern "C" bool __isObstacleColorable() {
    return ObstacleColorizer::ObstacleColorable;
}
