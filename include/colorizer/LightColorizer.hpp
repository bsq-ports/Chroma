#pragma once

#include "main.hpp"
#include "Chroma.hpp"
#include "lighting/ChromaLightSwitchEventEffect.hpp"

#include "sombrero/shared/ColorUtils.hpp"

#include "custom-types/shared/macros.hpp"

#include "GlobalNamespace/SimpleColorSO.hpp"
#include "UnityEngine/MonoBehaviour.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include <optional>

namespace Chroma {
class LightColorizer {
private:
  static int const COLOR_FIELDS = 4;

  int lightId;
  std::unordered_map<int, std::optional<Sombrero::FastColor>> _colors;
  std::array<SafePtrUnity<GlobalNamespace::SimpleColorSO>, 4> _originalColors;

  LightColorizer(ChromaLightSwitchEventEffect* lightSwitchEventEffect, GlobalNamespace::LightWithIdManager* lightManager);

  inline static std::vector<std::tuple<int, std::function<void(LightColorizer&)>>> _contracts;
  inline static std::vector<std::tuple<int, std::function<void(LightColorizer&)>>> _contractsByLightID;
  std::optional<std::unordered_map<int, std::vector<GlobalNamespace::ILightWithId*>>> LightsPropagationGrouped;

  void Refresh(std::optional<std::vector<GlobalNamespace::ILightWithId*>> const& selectLights) const;


public:
  ChromaLightSwitchEventEffect* _lightSwitchEventEffect;
  SafePtr<List<GlobalNamespace::ILightWithId*>> LightsSafePtr;

  using LightColorOptionalPalette = std::array<std::optional<Sombrero::FastColor>, 4>;
  using LightColorPalette = std::array<Sombrero::FastColor, 4>;

  friend class std::pair<int const, Chroma::LightColorizer>;
  friend class std::pair<int, LightColorizer>;
  friend class std::pair<int const, Chroma::LightColorizer>;

  inline static LightColorOptionalPalette GlobalColor{ std::nullopt, std::nullopt, std::nullopt, std::nullopt };

  inline static std::unordered_map<int, LightColorizer> Colorizers;
  inline static std::unordered_map<int, LightColorizer*> ColorizersByLightID;

  [[nodiscard]] ListW<GlobalNamespace::ILightWithId*> getLights() const {
    return LightsSafePtr.ptr();
  }

  static LightColorizer& New(ChromaLightSwitchEventEffect* lightSwitchEventEffect, GlobalNamespace::LightWithIdManager* lightManager);

  static void CreateLightColorizerContractByLightID(int lightId, std::function<void(LightColorizer&)> const& callback);

  static void CreateLightColorizerContract(GlobalNamespace::BasicBeatmapEventType type,
                                           std::function<void(LightColorizer&)> const& callback);

  static void CompleteContracts(ChromaLightSwitchEventEffect* chromaLightSwitchEventEffect);

  static void Reset();

  [[nodiscard]] std::unordered_map<int, std::vector<GlobalNamespace::ILightWithId*>> const& getLightsPropagationGrouped();

  [[nodiscard]] LightColorPalette getColor() const;

  static void GlobalColorize(std::optional<std::vector<GlobalNamespace::ILightWithId*>> const& selectLights,
                                    LightColorOptionalPalette const& colors) {
    GlobalColorize(true, selectLights, colors);
  }
  inline static void GlobalColorize(bool refresh, LightColorOptionalPalette const& colors) {
    GlobalColorize(refresh, std::nullopt, colors);
  }
  static void GlobalColorize(bool refresh, std::optional<std::vector<GlobalNamespace::ILightWithId*>> const& lights,
                             LightColorOptionalPalette const& colors);

  // dont use this please
  // cant be fucked to make an overload for this
  std::vector<GlobalNamespace::ILightWithId*> GetPropagationLightWithIds(std::vector<int> const& ids);
  [[nodiscard]] std::vector<GlobalNamespace::ILightWithId*> GetLightWithIds(std::vector<int> const& ids) const;

  inline void Colorize(std::optional<std::vector<GlobalNamespace::ILightWithId*>> const& selectLights,
                       LightColorOptionalPalette const& colors) {
    Colorize(true, selectLights, colors);
  }

  inline void Colorize(bool refresh, LightColorOptionalPalette const& colors) {
    Colorize(refresh, std::nullopt, colors);
  }

  void Colorize(bool refresh, std::optional<std::vector<GlobalNamespace::ILightWithId*>> const& selectLights,
                LightColorOptionalPalette const& colors);



  // extensions
  inline static LightColorizer* GetLightColorizer(GlobalNamespace::BasicBeatmapEventType BasicBeatmapEventType) {
    auto it = Colorizers.find(BasicBeatmapEventType.value__);
    if (it == Colorizers.end()) {
      return nullptr;
    }

    return &it->second;
  }

  inline static LightColorizer* GetLightColorizerLightID(int lightId) {
    auto it = ColorizersByLightID.find(lightId);
    if (it == ColorizersByLightID.end()) {
      return nullptr;
    }

    return it->second;
  }

  inline static void ColorizeLight(GlobalNamespace::BasicBeatmapEventType BasicBeatmapEventType, bool refresh,
                                   LightColorOptionalPalette const& colors) {
    auto colorizer = GetLightColorizer(BasicBeatmapEventType);
    CRASH_UNLESS(colorizer)->Colorize(refresh, colors);
  }


};
} // namespace Chroma
