#pragma once

#include "UnityEngine/Color.hpp"
#include "conditional-dependencies/shared/main.hpp"
#include "utils.hpp"

#include "GlobalNamespace/NoteController.hpp"

#include <optional>

namespace Chroma {

    /// TODO: NOT TESTED, USE WITH CAUTION
    /// Uses conditional dependency to call the method safely or return
    /// nullopt if it isn't found or if the original method returned nullopt
    class NoteAPI {
    public:

        /// TODO: unsure of this
        /// Gets the note type color or null if either Chroma is not setting the color or method was not found
        static std::optional<UnityEngine::Color> getNoteColorSafe(int colorType) noexcept {
            auto function = CondDep::Find<int, int>("chroma", "getNoteColorSafe");

            if (function) {
                // Returns the rgba value or -1 if it was a nullopt
                auto rgba = function.value()(colorType);

                if (rgba == -1) return std::nullopt;

                auto color = ColourManager::ColourFromInt(rgba);

                return std::make_optional(color);
            }

            return std::nullopt;
        }

        /// TODO: Unsure of whether this returns nullopt if Chroma sets the color or not.
        /// Gets the note color or null if either Chroma is not setting the color or method was not found
        static std::optional<UnityEngine::Color> getNoteControllerColorSafe(GlobalNamespace::NoteController* noteController, int colorType) noexcept {
            auto function = CondDep::Find<int, GlobalNamespace::NoteController*, int>("chroma", "getNoteControllerColorSafe");

            if (function) {
                // Returns the rgba value or -1 if it was a nullopt
                auto rgba = function.value()(noteController, colorType);

                if (rgba == -1) return std::nullopt;

                auto color = ColourManager::ColourFromInt(rgba);

                return std::make_optional(color);
            }

            return std::nullopt;
        }

        /// Sets the saber color if the method was found.
        static void setNoteColorSafe(GlobalNamespace::NoteController* nc, std::optional<UnityEngine::Color> color0, std::optional<UnityEngine::Color> color1) noexcept {
            auto function = CondDep::Find<void, GlobalNamespace::NoteController*, std::optional<UnityEngine::Color>, std::optional<UnityEngine::Color>>("chroma", "setNoteColorSafe");

            if (function) {
                function.value()(nc, color0, color1);
            }
        }
    };
}