
#pragma once

#include "lighting/LightIDTableManager.hpp"

namespace Chroma {
class DefaultEnvironment : public EnvironmentData {
public:
  EnvironmentLightDataT getEnvironmentLights() override;
};
} // namespace Chroma
