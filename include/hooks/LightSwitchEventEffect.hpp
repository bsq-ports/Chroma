#pragma once

#include "System/Collections/Generic/IEnumerator_1.hpp"
#include "colorizer/LightColorizer.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include "GlobalNamespace/LightSwitchEventEffect.hpp"

inline static std::optional<std::vector<GlobalNamespace::ILightWithId *>> OverrideLightWithIdActivation = std::nullopt;