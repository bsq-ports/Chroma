#pragma once

#include "main.hpp"

namespace Chroma {
    namespace Hooks {
        void LightPairRotationEventEffect();
        void LightRotationEventEffect();
        void LightSwitchEventEffect();
    }

    void InstallHooks();
}