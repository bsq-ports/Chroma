#pragma once

#include "GlobalNamespace/NoteControllerBase.hpp"

#include "UnityEngine/Color.hpp"

#include "colorizer/NoteColorizer.hpp"
#include "ChromaController.hpp"

#include <optional>

namespace Chroma {
    class ColorManagerColorForType {
    public:
        inline static std::optional<Sombrero::FastColor> _noteColorOverride;

        static void EnableColorOverride(GlobalNamespace::NoteControllerBase *noteController) {
            if (!ChromaController::DoColorizerSabers()) {
                _noteColorOverride = std::nullopt;
                return;
            }

            _noteColorOverride = NoteColorizer::GetNoteColorizer(noteController)->getColor();
        }

        static void DisableColorOverride() {
            _noteColorOverride = std::nullopt;
        }
    };
}


