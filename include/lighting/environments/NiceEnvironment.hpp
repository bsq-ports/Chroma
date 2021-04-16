#pragma once

#include "lighting/LightIDTableManager.hpp"

namespace Chroma {
    class NiceEnvironment : public EnvironmentData {
    public:
        EnvironmentLightDataT getEnvironmentLights() override;
    };
}


