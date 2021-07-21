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
    stretchableObstacle->materialPropertyBlockControllers->copy_to(_materialPropertyBlockControllers);

    auto trueObstacleControllerCast = il2cpp_utils::try_cast<ObstacleController>(obstacleController);
    if (trueObstacleControllerCast)
    {
        OriginalColor = (*trueObstacleControllerCast)->colorManager->get_obstaclesColor();
    }
    else
    {
        static auto white = Color::get_white();
        // Fallback
        OriginalColor = white;
    }
}

std::shared_ptr<ObstacleColorizer> ObstacleColorizer::New(GlobalNamespace::ObstacleControllerBase *obstacleController) {
    std::shared_ptr<ObstacleColorizer> obstacleColorizer(new ObstacleColorizer(obstacleController));

    Colorizers[obstacleController] = obstacleColorizer;

    return obstacleColorizer;
}

std::optional<UnityEngine::Color> ObstacleColorizer::GlobalColorGetter() {
    return GlobalColor;
}

void ObstacleColorizer::Reset() {
    GlobalColor = std::nullopt;
    Colorizers.clear();
    Colorizers = {};
}

void ObstacleColorizer::Refresh() {
    // We do not handle coloring in obstacle colorable
    if (ObstacleColorable) return;

    Color color = getColor();
    if (ChromaUtils::ColorEquals(color, _obstacleFrame->color))
    {
        return;
    }

    _obstacleFrame->color = color;
    _obstacleFrame->Refresh();
    if (_obstacleFakeGlow)
    {
        _obstacleFakeGlow->color = color;
        _obstacleFakeGlow->Refresh();
    }

    Color value = ChromaUtils::ColorMultiply(color, _addColorMultiplier);
    value.a = 0.0f;
    MOD_PTR_CACHE(static_cast<void (UnityEngine::MaterialPropertyBlock::*)(int, UnityEngine::Color)>(&UnityEngine::MaterialPropertyBlock::SetColor), SetColor, void, UnityEngine::MaterialPropertyBlock*, int, UnityEngine::Color)
    MOD_PTR_CACHE(&GlobalNamespace::MaterialPropertyBlockController::ApplyChanges, ApplyChanges, void, GlobalNamespace::MaterialPropertyBlockController*)

    for (auto& materialPropertyBlockController : _materialPropertyBlockControllers)
    {
        static auto white = Color::get_white();
        SetColor(materialPropertyBlockController->materialPropertyBlock, _addColorID(), value);
        SetColor(materialPropertyBlockController->materialPropertyBlock, _tintColorID(), ChromaUtils::ColorLerp(color, white, _obstacleCoreLerpToWhiteFactor));
        ApplyChanges(materialPropertyBlockController);
    }
}

void ObstacleColorizer::GlobalColorize(std::optional<UnityEngine::Color> color) {
    GlobalColor = color;
    for (auto& valuePair : Colorizers)
    {
        valuePair.second->Refresh();
    }
}

int ObstacleColorizer::_tintColorID() {
    static auto id = UnityEngine::Shader::PropertyToID(il2cpp_utils::newcsstr("_TintColor"));
    return id;
}

int ObstacleColorizer::_addColorID() {
    static auto id = UnityEngine::Shader::PropertyToID(il2cpp_utils::newcsstr("_AddColor"));
    return id;
}
