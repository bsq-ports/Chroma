
#pragma once

#include "lighting/LightIDTableManager.hpp"

namespace Chroma {
class InterscopeEnvironment : public EnvironmentData {
public:
  EnvironmentLightDataT getEnvironmentLights() override;
};
} // namespace Chroma
