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
        // Fallback
        OriginalColor = Color::get_white();
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
}

void ObstacleColorizer::Refresh() {
    Color color = getColor();
    if (color == _obstacleFrame->color)
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

    Color value = color * _addColorMultiplier;
    value.a = 0.0f;
    for (auto& materialPropertyBlockController : _materialPropertyBlockControllers)
    {
        materialPropertyBlockController->materialPropertyBlock->SetColor(_addColorID(), value);
        materialPropertyBlockController->materialPropertyBlock->SetColor(_tintColorID(), Color::Lerp(color, Color::get_white(), _obstacleCoreLerpToWhiteFactor));
        materialPropertyBlockController->ApplyChanges();
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
