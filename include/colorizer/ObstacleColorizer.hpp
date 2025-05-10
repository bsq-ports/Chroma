#pragma once

#include "main.hpp"
#include "Chroma.hpp"
#include "ObjectColorizer.hpp"

#include "GlobalNamespace/ParametricBoxFakeGlowController.hpp"
#include "GlobalNamespace/ParametricBoxFrameController.hpp"
#include "GlobalNamespace/MaterialPropertyBlockController.hpp"
#include "UnityEngine/MaterialPropertyBlock.hpp"

#include <vector>
#include <string>
#include <optional>
#include <unordered_map>

#include "custom-types/shared/macros.hpp"

namespace GlobalNamespace {
class ParametricBoxFrameController;
class ParametricBoxFakeGlowController;
class MaterialPropertyBlockController;
class ObstacleControllerBase;
} // namespace GlobalNamespace

namespace Chroma {
class ObstacleColorizer : public ObjectColorizer<ObstacleColorizer> {
private:
  friend class ObjectColorizer<ObstacleColorizer>;

  static int _tintColorID();
  static int _addColorID();

  GlobalNamespace::ParametricBoxFrameController* _obstacleFrame;
  GlobalNamespace::ParametricBoxFakeGlowController* _obstacleFakeGlow;
  float _addColorMultiplier;
  float _obstacleCoreLerpToWhiteFactor;
  ArrayW<UnityW<GlobalNamespace::MaterialPropertyBlockController>> _materialPropertyBlockControllers;
  GlobalNamespace::ObstacleControllerBase* obstacleController;

  explicit ObstacleColorizer(GlobalNamespace::ObstacleControllerBase* obstacleController);

protected:
  static std::optional<Sombrero::FastColor> GlobalColorGetter() {
    return GlobalColor;
  }

  void Refresh() {
    Sombrero::FastColor const& color = getColor();
    auto frameColor = Sombrero::FastColor(_obstacleFrame->color);
    if (color == frameColor) {
      return;
    }

    ObstacleColorChanged.invoke(obstacleController, color);

    // We do not handle coloring in obstacle colorable
    if (ObstacleColorable) return;

    _obstacleFrame->color = color;
    static auto Refresh = FPtrWrapper<&GlobalNamespace::ParametricBoxFakeGlowController::Refresh>::get();
    _obstacleFrame->Refresh();
    if (_obstacleFakeGlow) {
      _obstacleFakeGlow->color = color;
      Refresh(_obstacleFakeGlow);
    }

    Sombrero::FastColor value = color * _addColorMultiplier;
    value.a = 0.0f;
    static auto ApplyChanges = FPtrWrapper<&GlobalNamespace::MaterialPropertyBlockController::ApplyChanges>::get();
    static auto SetColor =
        FPtrWrapper<static_cast<void (UnityEngine::MaterialPropertyBlock::*)(int, UnityEngine::Color)>(
            &UnityEngine::MaterialPropertyBlock::SetColor)>::get();

    for (auto materialPropertyBlockController : _materialPropertyBlockControllers) {
      if (!materialPropertyBlockController->materialPropertyBlock) {
        continue;
      }

      Sombrero::FastColor white = Sombrero::FastColor::white();
      SetColor(materialPropertyBlockController->materialPropertyBlock, _addColorID(), value);
      SetColor(materialPropertyBlockController->materialPropertyBlock, _tintColorID(),
               Sombrero::FastColor::Lerp(color, white, _obstacleCoreLerpToWhiteFactor));
      ApplyChanges(materialPropertyBlockController);
    }
  }

public:
  inline static bool ObstacleColorable = false;
  inline static UnorderedEventCallback<GlobalNamespace::ObstacleControllerBase*, Sombrero::FastColor const&>
      ObstacleColorChanged;

  ObstacleColorizer(ObstacleColorizer const&) = delete;
  friend class std::pair<GlobalNamespace::ObstacleControllerBase const*, ObstacleColorizer>;
  friend class std::pair<GlobalNamespace::ObstacleControllerBase const* const, Chroma::ObstacleColorizer>;

  static ObstacleColorizer& New(GlobalNamespace::ObstacleControllerBase* obstacleController);

  inline static std::unordered_map<GlobalNamespace::ObstacleControllerBase const*, ObstacleColorizer> Colorizers;
  inline static std::optional<Sombrero::FastColor> GlobalColor;

  static void GlobalColorize(std::optional<Sombrero::FastColor> const& color);

  static void Reset();

  // extensions
  inline static ObstacleColorizer* GetObstacleColorizer(GlobalNamespace::ObstacleControllerBase* obstacleController) {
    auto it = Colorizers.find(obstacleController);
    if (it == Colorizers.end()) return nullptr;

    return &it->second;
  }

  inline static void ColorizeObstacle(GlobalNamespace::ObstacleControllerBase* obstacleControllerBase,
                                      std::optional<Sombrero::FastColor> const& color) {
    CRASH_UNLESS(GetObstacleColorizer(obstacleControllerBase))->Colorize(color);
  }
};
} // namespace Chroma