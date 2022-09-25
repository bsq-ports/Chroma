#pragma once

#include "ChromaController.hpp"

#include "GlobalNamespace/NoteControllerBase.hpp"

#include "colorizer/NoteColorizer.hpp"

#include <optional>

namespace Chroma {
    class ColorManagerColorForType {
    public:
        inline static std::optional<Sombrero::FastColor> _noteColorOverride;

        static void EnableColorOverride(GlobalNamespace::NoteControllerBase *noteController) {
            if (ChromaController::DoColorizerSabers()) {
                _noteColorOverride = NoteColorizer::GetNoteColorizer(noteController)->getColor();
            } else {
                _noteColorOverride = std::nullopt;
            }
        }

        static void DisableColorOverride() {
            _noteColorOverride = std::nullopt;
        }
    };
}


