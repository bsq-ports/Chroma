
#pragma once

#include "lighting/LightIDTableManager.hpp"


namespace Chroma {
    class BillieEnvironment : public EnvironmentData {
    public:
        EnvironmentLightDataT getEnvironmentLights() override;
    };
}
