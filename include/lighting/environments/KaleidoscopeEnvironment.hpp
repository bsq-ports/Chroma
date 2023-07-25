
#pragma once

#include "lighting/LightIDTableManager.hpp"

namespace Chroma {
class KaleidoscopeEnvironment : public EnvironmentData {
public:
  EnvironmentLightDataT getEnvironmentLights() override;
};
} // namespace Chroma
