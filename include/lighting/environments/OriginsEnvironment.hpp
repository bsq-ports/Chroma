#pragma once

#include "lighting/LightIDTableManager.hpp"

namespace Chroma {
    class OriginsEnvironment : public EnvironmentData {
    public:
        EnvironmentLightDataT getEnvironmentLights() override;
    };
}


