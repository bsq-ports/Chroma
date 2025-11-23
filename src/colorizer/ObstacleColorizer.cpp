#include "Chroma.hpp"

#include "UnityEngine/Shader.hpp"
#include "GlobalNamespace/StretchableObstacle.hpp"
#include "GlobalNamespace/ColorManager.hpp"
#include "GlobalNamespace/ObstacleControllerBase.hpp"
#include "GlobalNamespace/ObstacleController.hpp"

#include <unordered_map>
#include "colorizer/ObstacleColorizer.hpp"
#include "utils/ChromaUtils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

using namespace GlobalNamespace;
using namespace UnityEngine;

using namespace Chroma;

ObstacleColorizer::ObstacleColorizer(GlobalNamespace::ObstacleControllerBase* obstacleController) : obstacleController(obstacleController) {
  auto* stretchableObstacle = obstacleController->GetComponent<StretchableObstacle*>();
  _obstacleFrame = stretchableObstacle->_obstacleFrame;
  _obstacleFakeGlow = stretchableObstacle->_obstacleFakeGlow;
  _addColorMultiplier = stretchableObstacle->_addColorMultiplier;
  _obstacleCoreLerpToWhiteFactor = stretchableObstacle->_obstacleCoreLerpToWhiteFactor;
  _materialPropertyBlockControllers = stretchableObstacle->_materialPropertyBlockControllers;

  auto trueObstacleControllerCast = il2cpp_utils::try_cast<ObstacleController>(obstacleController);
  if (trueObstacleControllerCast) {
    static auto get_obstaclesColor = FPtrWrapper<&ColorManager::get_obstaclesColor>::get();
    OriginalColor = get_obstaclesColor((*trueObstacleControllerCast)->_colorManager);
  } else {
    static auto white = Sombrero::FastColor::white();
    // Fallback
    OriginalColor = white;
  }
}

ObstacleColorizer& ObstacleColorizer::New(GlobalNamespace::ObstacleControllerBase* obstacleController) {
  return Colorizers.try_emplace(obstacleController, obstacleController).first->second;
}

void ObstacleColorizer::Reset() {
  GlobalColor = std::nullopt;
  Colorizers.clear();
  ObstacleColorChanged.clear();
}

void ObstacleColorizer::GlobalColorize(std::optional<Sombrero::FastColor> const& color) {
  GlobalColor = color;
  for (auto& [_, colorizer] : Colorizers) {
    colorizer.Refresh();
  }
}

int _tintColorID() {
  static auto id = UnityEngine::Shader::PropertyToID("_TintColor");
  return id;
}

int _addColorID() {
  static auto id = UnityEngine::Shader::PropertyToID("_AddColor");
  return id;
}

void ObstacleColorizer::Refresh() {
  Sombrero::FastColor const& color = getColor();
  auto frameColor = Sombrero::FastColor(_obstacleFrame->color);
  if (color == frameColor) {
    return;
  }

  ObstacleColorChanged.invoke(obstacleController, color);

  // We do not handle coloring in obstacle colorable
  if (ObstacleColorable) {
    return;
  }

  // for Noodle to apply the color in Init
  auto obstacleController = il2cpp_utils::try_cast<ObstacleController>(this->obstacleController);
  if (obstacleController) {
    obstacleController.value()->_color = color;
  }

  _obstacleFrame->color = color;
  _obstacleFrame->Refresh();
  
  if (_obstacleFakeGlow) {
    _obstacleFakeGlow->color = color;
    static auto Refresh = FPtrWrapper<&GlobalNamespace::ParametricBoxFakeGlowController::Refresh>::get();
    Refresh(_obstacleFakeGlow);
  }

  Sombrero::FastColor value = color * _addColorMultiplier;
  value.a = 0.0F;

  static auto ApplyChanges = FPtrWrapper<&GlobalNamespace::MaterialPropertyBlockController::ApplyChanges>::get();
  static auto SetColor = FPtrWrapper<static_cast<void (UnityEngine::MaterialPropertyBlock::*)(int, UnityEngine::Color)>(
      &UnityEngine::MaterialPropertyBlock::SetColor)>::get();

  for (auto materialPropertyBlockController : _materialPropertyBlockControllers) {
    if (!materialPropertyBlockController  || !materialPropertyBlockController->materialPropertyBlock) {
      continue;
    }

    Sombrero::FastColor white = Sombrero::FastColor::white();
    
    SetColor(materialPropertyBlockController->materialPropertyBlock, _addColorID(), value);
    SetColor(materialPropertyBlockController->materialPropertyBlock, _tintColorID(),
             Sombrero::FastColor::Lerp(color, white, _obstacleCoreLerpToWhiteFactor));
    ApplyChanges(materialPropertyBlockController);
  }
}