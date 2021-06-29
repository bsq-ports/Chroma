
#pragma once

#include "lighting/LightIDTableManager.hpp"


namespace Chroma {
    class GreenDayGrenadeEnvironment : public EnvironmentData {
    public:
        EnvironmentLightDataT getEnvironmentLights() override;
    };
}
