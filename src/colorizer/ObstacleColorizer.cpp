#include "Chroma.hpp"

#include "UnityEngine/Shader.hpp"
#include "UnityEngine/MaterialPropertyBlock.hpp"

#include "GlobalNamespace/MaterialPropertyBlockController.hpp"
#include "GlobalNamespace/ParametricBoxFrameController.hpp"
#include "GlobalNamespace/ParametricBoxFakeGlowController.hpp"
#include "GlobalNamespace/StretchableObstacle.hpp"
#include "GlobalNamespace/ColorManager.hpp"



#include <unordered_map>
#include "colorizer/ObstacleColorizer.hpp"
#include "utils/ChromaUtils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"


using namespace GlobalNamespace;
using namespace UnityEngine;

using namespace Chroma;


ObstacleColorizer::ObstacleColorizer(GlobalNamespace::ObstacleControllerBase *obstacleController) {
    auto stretchableObstacle = obstacleController->GetComponent<StretchableObstacle*>();
    _obstacleFrame = stretchableObstacle->obstacleFrame;
    _obstacleFakeGlow = stretchableObstacle->obstacleFakeGlow;
    _addColorMultiplier = stretchableObstacle->addColorMultiplier;
    _obstacleCoreLerpToWhiteFactor = stretchableObstacle->obstacleCoreLerpToWhiteFactor;
    _materialPropertyBlockControllers = stretchableObstacle->materialPropertyBlockControllers;

    auto trueObstacleControllerCast = il2cpp_utils::try_cast<ObstacleController>(obstacleController);
    if (trueObstacleControllerCast)
    {
        static auto get_obstaclesColor = FPtrWrapper<&ColorManager::get_obstaclesColor>::get();
        OriginalColor = get_obstaclesColor((*trueObstacleControllerCast)->colorManager);
    }
    else
    {
        static auto white = Sombrero::FastColor::white();
        // Fallback
        OriginalColor = white;
    }
}

ObstacleColorizer& ObstacleColorizer::New(GlobalNamespace::ObstacleControllerBase *obstacleController) {
    return Colorizers.try_emplace(obstacleController, obstacleController).first->second;
}

void ObstacleColorizer::Reset() {
    GlobalColor = std::nullopt;
    Colorizers.clear();
}

void ObstacleColorizer::GlobalColorize(std::optional<Sombrero::FastColor> const& color) {
    GlobalColor = color;
    for (auto& [_, colorizer] : Colorizers)
    {
        colorizer.Refresh();
    }
}

int ObstacleColorizer::_tintColorID() {
    static auto id = UnityEngine::Shader::PropertyToID("_TintColor");
    return id;
}

int ObstacleColorizer::_addColorID() {
    static auto id = UnityEngine::Shader::PropertyToID("_AddColor");
    return id;
}
