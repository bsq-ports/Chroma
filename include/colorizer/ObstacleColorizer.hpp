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
class ObstacleColorizer : public ObjectColorizer {
  friend class std::pair<GlobalNamespace::ObstacleControllerBase const*, ObstacleColorizer>;
  friend class std::pair<GlobalNamespace::ObstacleControllerBase const* const, ObstacleColorizer>;

private:
  GlobalNamespace::ParametricBoxFrameController* _obstacleFrame;
  GlobalNamespace::ParametricBoxFakeGlowController* _obstacleFakeGlow;
  float _addColorMultiplier;
  float _obstacleCoreLerpToWhiteFactor;
  ArrayW<UnityW<GlobalNamespace::MaterialPropertyBlockController>> _materialPropertyBlockControllers;
  GlobalNamespace::ObstacleControllerBase* obstacleController;


  explicit ObstacleColorizer(GlobalNamespace::ObstacleControllerBase* obstacleController);

protected:
  [[nodiscard]] constexpr static std::optional<Sombrero::FastColor> getGlobalColorStatic() {
    return GlobalColor;
  }
  [[nodiscard]] std::optional<Sombrero::FastColor> getGlobalColor() const final {
    return GlobalColor;
  }

  void Refresh() final;

public:
  inline static bool ObstacleColorable = false;
  inline static UnorderedEventCallback<GlobalNamespace::ObstacleControllerBase*, Sombrero::FastColor const&> ObstacleColorChanged;

  static ObstacleColorizer& New(GlobalNamespace::ObstacleControllerBase* obstacleController);

  inline static std::unordered_map<GlobalNamespace::ObstacleControllerBase const*, ObstacleColorizer> Colorizers;
  inline static std::optional<Sombrero::FastColor> GlobalColor;

  static void GlobalColorize(std::optional<Sombrero::FastColor> const& color);

  static void Reset();

  // extensions
  inline static ObstacleColorizer* GetObstacleColorizer(GlobalNamespace::ObstacleControllerBase* obstacleController) {
    auto it = Colorizers.find(obstacleController);
    if (it == Colorizers.end()) {
      return nullptr;
    }

    return &it->second;
  }

  inline static void ColorizeObstacle(GlobalNamespace::ObstacleControllerBase* obstacleControllerBase,
                                      std::optional<Sombrero::FastColor> const& color) {
    CRASH_UNLESS(GetObstacleColorizer(obstacleControllerBase))->Colorize(color);
  }
};
} // namespace Chroma