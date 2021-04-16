#pragma once

#include "lighting/LightIDTableManager.hpp"

namespace Chroma {
    class BigMirrorEnvironment : public EnvironmentData {
    public:
        EnvironmentLightDataT getEnvironmentLights() override;
    };
}


