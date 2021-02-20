#pragma once

#include "main.hpp"
#include "UnityEngine/Color.hpp"
#include <optional>

namespace Chroma {
    namespace Hooks {
        void LightPairRotationEventEffect();
        void LightRotationEventEffect();
        void LightSwitchEventEffect();
    }

    namespace CustomData {
        struct NoteData {
            std::optional<UnityEngine::Color> _color0;
            std::optional<UnityEngine::Color> _color1;
        };
    }

    void InstallHooks();
}