
#pragma once

#include "lighting/LightIDTableManager.hpp"


namespace Chroma {
    class MonstercatEnvironment : public EnvironmentData {
    public:
        EnvironmentLightDataT getEnvironmentLights() override;
    };
}
