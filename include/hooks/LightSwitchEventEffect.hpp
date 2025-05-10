#pragma once

namespace GlobalNamespace {
class ILightWithId;
class LightSwitchEventEffect;
class LightWithIdManager;
} // namespace GlobalNamespace

#include <optional>
#include <vector>

class LightSwitchEventEffectHolder {
public:
  inline static std::optional<std::vector<int>> LightIDOverride = std::nullopt;
  inline static std::optional<std::vector<GlobalNamespace::ILightWithId*>> LegacyLightOverride = std::nullopt;
};
