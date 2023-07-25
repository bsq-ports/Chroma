
#pragma once

#include "lighting/LightIDTableManager.hpp"

namespace Chroma {
class DragonsEnvironment : public EnvironmentData {
public:
  EnvironmentLightDataT getEnvironmentLights() override;
};
} // namespace Chroma
