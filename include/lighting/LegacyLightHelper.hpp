#pragma once

#include "GlobalNamespace/BasicBeatmapEventData.hpp"


#include "sombrero/shared/ColorUtils.hpp"

#include <unordered_map>
#include <vector>
#include <optional>
#include <utility>

namespace Chroma {
// Please let me delete this whole class
class LegacyLightHelper {
public:
  typedef std::unordered_map<int, std::vector<std::pair<float, Sombrero::FastColor>>> ColorMap;

  // internal
  inline const static int RGB_INT_OFFSET = 2000000000;

  // internal
  static ColorMap LegacyColorEvents;

  // internal
  static void Activate(std::span<GlobalNamespace::BasicBeatmapEventData*> eventData);

  // internal
  static std::optional<Sombrero::FastColor> GetLegacyColor(GlobalNamespace::BasicBeatmapEventData* beatmapEventData);

private:
  static constexpr Sombrero::FastColor ColorFromInt(int rgb);
};
} // namespace Chroma