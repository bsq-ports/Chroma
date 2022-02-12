#pragma once

#include "UnityEngine/Color.hpp"
#include "conditional-dependencies/shared/main.hpp"
#include "utils.hpp"

#include "GlobalNamespace/BombNoteController.hpp"

#include <optional>

namespace Chroma {

    /// TODO: NOT TESTED, USE WITH CAUTION
    /// Uses conditional dependency to call the method safely or return
    /// nullopt if it isn't found or if the original method returned nullopt
    class BombAPI {
    public:

        /// TODO: unsure of this
        /// Gets the Bomb type color or null if either Chroma is not setting the color or method was not found
        static std::optional<Sombrero::FastColor> getBombColorSafe() noexcept {
            static auto function = CondDeps::Find<OptColor>(CHROMA_ID, "getBombColorSafe");

            if (function) {
                // Returns the color struct
                auto optColor = function.value()();

                if (!optColor.isSet) return std::nullopt;

                return optColor.getColor();
            }

            return std::nullopt;
        }


        /// Gets the Bomb color or null if Chroma method was not found
        /// If Chroma is not setting the color, it returns the game's colors
        /// THIS WILL NOT PLAY NICELY WITH MIRRORED Bomb CONTROLLER
        static std::optional<Sombrero::FastColor> getBombNoteControllerOverrideColorSafe(GlobalNamespace::BombNoteController* BombNoteController) noexcept {
            static auto function = CondDeps::Find<OptColor, GlobalNamespace::BombNoteController*>(CHROMA_ID, "getBombNoteControllerOverrideColorSafe");

            if (function) {
                // Returns the color struct
                auto optColor = function.value()(BombNoteController);

                if (!optColor.isSet) return std::nullopt;

                return optColor.getColor();
            }

            return std::nullopt;
        }

        /// Gets the Bomb color or null if Chroma method was not found
        /// If Chroma is not setting the color, it returns std::nullopt
        /// THIS WILL NOT PLAY NICELY WITH MIRRORED Bomb CONTROLLER
        static std::optional<Sombrero::FastColor> getBombNoteControllerColorSafe(GlobalNamespace::BombNoteController* BombNoteController) noexcept {
            static auto function = CondDeps::Find<OptColor, GlobalNamespace::BombNoteController*>(CHROMA_ID, "getBombNoteControllerColorSafe");

            if (function) {
                // Returns the color struct
                auto optColor = function.value()(BombNoteController);

                if (!optColor.isSet) return std::nullopt;

                return optColor.getColor();
            }

            return std::nullopt;
        }

        /// Sets the Bomb color if the method was found.
        static void setBombColorSafe(GlobalNamespace::BombNoteController* nc, std::optional<Sombrero::FastColor> color0) noexcept {
            static auto function = CondDeps::Find<void, GlobalNamespace::BombNoteController*, std::optional<Sombrero::FastColor>>(CHROMA_ID, "setBombColorSafe");

            if (function) {
                function.value()(nc, color0);
            }
        }

        /// Sets the Bomb color if the method was found.
        static void setGlobalBombColorSafe(std::optional<Sombrero::FastColor> color0) noexcept {
            static auto function = CondDeps::Find<void, std::optional<Sombrero::FastColor>>(CHROMA_ID, "setGlobalBombColorSafe");

            if (function) {
                function.value()(color0);
            }
        }

        /// Sets if the Bomb is colorable. If this is set to true, Chroma will NOT color the Bomb and
        /// instead the work should be done by the mod handling coloring Bombs usually a custom Bomb mod
        static void setBombColorable(bool colorable) {
            static auto function = CondDeps::Find<void, bool>(CHROMA_ID, "setBombColorable");

            if (function) {
                function.value()(colorable);
            }
        }

        /// Checks if the Bomb is colorable. If this is set to true, Chroma will NOT color the Bomb and
        /// instead the work should be done by the mod handling coloring Bombs usually a custom Bomb mod
        static std::optional<bool> isBombColorable() {
            static auto function = CondDeps::Find<bool>(CHROMA_ID, "isBombColorable");

            if (function) {
                return function.value()();
            }

            return std::nullopt;
        }

        /// This retrieves the callback used for Saber Color changed.
        ///
        /// \return
        using BombCallback = UnorderedEventCallback<GlobalNamespace::NoteControllerBase*, Sombrero::FastColor const&>;
        static std::optional<std::reference_wrapper<BombCallback>> getBombChangedColorCallbackSafe() {
            static auto function = CondDeps::Find<BombCallback*>(CHROMA_ID, "getBombChangedColorCallbackSafe");

            /// Oh boi what have I done
            if (function) {
                BombCallback& callback = *function.value()();
                return std::make_optional(std::ref(callback));
            }

            return std::nullopt;
        }
    };
}