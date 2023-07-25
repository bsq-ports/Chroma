
#pragma once

#include "lighting/LightIDTableManager.hpp"

namespace Chroma {
class TriangleEnvironment : public EnvironmentData {
public:
  EnvironmentLightDataT getEnvironmentLights() override;
};
} // namespace Chroma
