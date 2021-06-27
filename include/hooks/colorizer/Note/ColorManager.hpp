#pragma once

#include "GlobalNamespace/NoteControllerBase.hpp"

#include "UnityEngine/Color.hpp"

#include "colorizer/NoteColorizer.hpp"

#include <optional>

namespace Chroma {
    class ColorManagerColorForType {
    public:
        inline static std::optional<UnityEngine::Color> _noteColorOverride;

        static void EnableColorOverride(GlobalNamespace::NoteControllerBase *noteController) {
            _noteColorOverride = NoteColorizer::GetNoteColorizer(noteController)->getColor();
        }

        static void DisableColorOverride() {
            _noteColorOverride = std::nullopt;
        }
    };
}


