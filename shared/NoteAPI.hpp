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
            static auto function = CondDep::Find<OptColor, int>(CHROMA_ID, "getNoteColorSafe");

            if (function) {
                // Returns the color struct
                auto optColor = function.value()(colorType);

                if (!optColor.isSet) return std::nullopt;

                return optColor.getColor();
            }

            return std::nullopt;
        }


        /// Gets the note color or null if Chroma method was not found
        /// If Chroma is not setting the color, it returns the game's colors
        /// THIS WILL NOT PLAY NICELY WITH MIRRORED NOTE CONTROLLER
        static std::optional<UnityEngine::Color> getNoteControllerOverrideColorSafe(GlobalNamespace::NoteController* noteController, int colorType) noexcept {
            static auto function = CondDep::Find<OptColor, GlobalNamespace::NoteController*, int>(CHROMA_ID, "getNoteControllerOverrideColorSafe");

            if (function) {
                // Returns the color struct
                auto optColor = function.value()(noteController, colorType);

                if (!optColor.isSet) return std::nullopt;

                return optColor.getColor();
            }

            return std::nullopt;
        }

        /// Gets the note color or null if Chroma method was not found
        /// If Chroma is not setting the color, it returns std::nullopt
        /// THIS WILL NOT PLAY NICELY WITH MIRRORED NOTE CONTROLLER
        static std::optional<UnityEngine::Color> getNoteControllerColorSafe(GlobalNamespace::NoteController* noteController, int colorType) noexcept {
            static auto function = CondDep::Find<OptColor, GlobalNamespace::NoteController*, int>(CHROMA_ID, "getNoteControllerColorSafe");

            if (function) {
                // Returns the color struct
                auto optColor = function.value()(noteController, colorType);

                if (!optColor.isSet) return std::nullopt;

                return optColor.getColor();
            }

            return std::nullopt;
        }

        /// Sets the note color if the method was found.
        static void setNoteColorSafe(GlobalNamespace::NoteController* nc, std::optional<UnityEngine::Color> color0) noexcept {
            static auto function = CondDep::Find<void, GlobalNamespace::NoteController*, std::optional<UnityEngine::Color>>(CHROMA_ID, "setNoteColorSafe");

            if (function) {
                function.value()(nc, color0);
            }
        }

        /// Sets the note color if the method was found.
        static void setGlobalNoteColorSafe(std::optional<UnityEngine::Color> color0, std::optional<UnityEngine::Color> color1) noexcept {
            static auto function = CondDep::Find<void, std::optional<UnityEngine::Color>,  std::optional<UnityEngine::Color>>(CHROMA_ID, "setGlobalNoteColorSafe");

            if (function) {
                function.value()(color0, color1);
            }
        }

        /// Sets if the note is colorable. If this is set to true, Chroma will NOT color the note and
        /// instead the work should be done by the mod handling coloring notes usually a custom note mod
        static void setNoteColorable(bool colorable) {
            static auto function = CondDep::Find<void, bool>(CHROMA_ID, "setSaberColorable");

            if (function) {
                function.value()(colorable);
            }
        }

        /// Checks if the note is colorable. If this is set to true, Chroma will NOT color the note and
        /// instead the work should be done by the mod handling coloring notes usually a custom note mod
        static std::optional<bool> isNoteColorable() {
            static auto function = CondDep::Find<bool>(CHROMA_ID, "isSaberColorable");

            if (function) {
                return function.value()();
            }

            return std::nullopt;
        }
    };
}