#pragma once

#include "UnityEngine/Color.hpp"
#include "conditional-dependencies/shared/main.hpp"
#include "utils.hpp"

#include <optional>

namespace Chroma {

    /// Uses conditional dependency to call the method safely or return
    /// nullopt if it isn't found or if the original method returned nullopt
    class SaberAPI {
    public:

        /// Gets the saber color or null if either Chroma is not setting the color or method was not found
        static std::optional<UnityEngine::Color> getSaberColorSafe(int saberType) noexcept {
            auto function = CondDep::Find<int, int>("chroma", "getSaberColorSafe");

            if (function) {
                // Returns the rgba value or -1 if it was a nullopt
                auto rgba = function.value()(saberType);

                if (rgba == -1) return std::nullopt;

                auto color = ColourManager::ColourFromInt(rgba);

                return std::make_optional(color);
            }

            return std::nullopt;
        }

        /// Sets the saber color if the method was found.
        static void setSaberColorSafe(int saberType, std::optional<UnityEngine::Color> color) noexcept {
            auto function = CondDep::Find<void, int, std::optional<UnityEngine::Color>>("chroma", "setSaberColorSafe");

            if (function) {
                function.value()(saberType, color);
            }
        }
    };
}