#pragma once

#include "lighting/LightIDTableManager.hpp"

namespace Chroma {
    class GlassDesertEnvironment : public EnvironmentData {
    public:
        EnvironmentLightDataT getEnvironmentLights() override;
    };
}


