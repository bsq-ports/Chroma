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
    _materialPropertyBlockControllers = stretchableObstacle->materialPropertyBlockControllers->ref_to();

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

std::shared_ptr<ObstacleColorizer> ObstacleColorizer::New(GlobalNamespace::ObstacleControllerBase *obstacleController) {
    std::shared_ptr<ObstacleColorizer> obstacleColorizer(new ObstacleColorizer(obstacleController));

    Colorizers[obstacleController] = obstacleColorizer;

    return obstacleColorizer;
}

std::optional<Sombrero::FastColor> ObstacleColorizer::GlobalColorGetter() {
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

    Sombrero::FastColor const& color = getColor();
    if (color == Sombrero::FastColor(_obstacleFrame->color))
    {
        return;
    }

    _obstacleFrame->color = color;
    static auto Refresh = FPtrWrapper<&ParametricBoxFakeGlowController::Refresh>::get();
    _obstacleFrame->Refresh();
    if (_obstacleFakeGlow)
    {
        _obstacleFakeGlow->color = color;
        Refresh(_obstacleFakeGlow);
    }

    Sombrero::FastColor value = color * _addColorMultiplier;
    value.a = 0.0f;
    static auto ApplyChanges = FPtrWrapper<&GlobalNamespace::MaterialPropertyBlockController::ApplyChanges>::get();
    static auto SetColor = FPtrWrapper<static_cast<void (UnityEngine::MaterialPropertyBlock::*)(int, UnityEngine::Color)>(&UnityEngine::MaterialPropertyBlock::SetColor)>::get();

    for (auto& materialPropertyBlockController : _materialPropertyBlockControllers)
    {
        Sombrero::FastColor white = Sombrero::FastColor::white();
        SetColor(materialPropertyBlockController->materialPropertyBlock, _addColorID(), value);
        SetColor(materialPropertyBlockController->materialPropertyBlock, _tintColorID(), Sombrero::FastColor::Lerp(color, white, _obstacleCoreLerpToWhiteFactor));
        ApplyChanges(materialPropertyBlockController);
    }
}

void ObstacleColorizer::GlobalColorize(std::optional<Sombrero::FastColor> const& color) {
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
