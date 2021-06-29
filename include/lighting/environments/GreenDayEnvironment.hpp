
#pragma once

#include "lighting/LightIDTableManager.hpp"


namespace Chroma {
    class GreenDayEnvironment : public EnvironmentData {
    public:
        EnvironmentLightDataT getEnvironmentLights() override;
    };
}
