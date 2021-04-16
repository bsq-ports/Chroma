#pragma once

#include "lighting/LightIDTableManager.hpp"

namespace Chroma {
    class BTSEnvironment : public EnvironmentData {
    public:
        EnvironmentLightDataT getEnvironmentLights() override;
    };
}


