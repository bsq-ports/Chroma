#pragma once

#include "GlobalNamespace/ILightWithId.hpp"

#include <optional>
#include <vector>

class LightSwitchEventEffectHolder {
    public:
        inline static std::optional<std::vector<int>> LightIDOverride = std::nullopt;
        inline static std::optional<std::vector<GlobalNamespace::ILightWithId *>> LegacyLightOverride = std::nullopt;
};
