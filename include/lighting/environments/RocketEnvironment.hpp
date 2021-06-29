
#pragma once

#include "lighting/LightIDTableManager.hpp"


namespace Chroma {
    class RocketEnvironment : public EnvironmentData {
    public:
        EnvironmentLightDataT getEnvironmentLights() override;
    };
}
