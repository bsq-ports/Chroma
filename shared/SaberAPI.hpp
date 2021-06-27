#pragma once

#include "UnityEngine/Color.hpp"
#include "GlobalNamespace/SaberType.hpp"
#include "conditional-dependencies/shared/main.hpp"
#include "utils.hpp"
#include "utils/EventCallback.hpp"

#include <optional>
#include <functional>
#include <utility>

namespace Chroma {

    /// Uses conditional dependency to call the method safely or return
    /// nullopt if it isn't found or if the original method returned nullopt
    class SaberAPI {
    public:

        /// Gets the saber color or null if either Chroma is not setting the color or method was not found
        static std::optional<UnityEngine::Color> getSaberColorSafe(int saberType) noexcept {
            auto function = CondDep::Find<OptColor, int>(CHROMA_ID, "getSaberColorSafe");

            if (function) {
                // Returns the color struct
                auto optColor = function.value()(saberType);

                if (!optColor.isSet) return std::nullopt;

                return optColor.getColor();
            }

            return std::nullopt;
        }

        using ColorPair = std::pair<std::optional<UnityEngine::Color>, std::optional<UnityEngine::Color>>;
        using ColorOptPair = ExternPair<OptColor, OptColor>;
        /// Gets the saber color or null if either Chroma is not setting the color or method was not found
        static ColorPair getSabersColorSafe() noexcept {
            auto function = CondDep::Find<ColorOptPair>(CHROMA_ID, "getSabersColorSafe");

            if (function) {
                // Returns the two rgba values
                auto pair = function.value()();

                return {pair.first.isSet ? std::make_optional(pair.first.getColor()) : std::nullopt,
                        pair.second.isSet ? std::make_optional(pair.second.getColor()) : std::nullopt};
            }

            return {std::nullopt, std::nullopt};
        }

        /// Sets the saber color if the method was found.
        static void setSaberColorSafe(int saberType, UnityEngine::Color color) noexcept {
            auto function = CondDep::Find<void, int, UnityEngine::Color>(CHROMA_ID, "setSaberColorSafe");

            if (function) {
                function.value()(saberType, color);
            }
        }

        /// This registers a callback that is called whenever the saber color changes
        /// Do note however that every time a scene changes the callback is erased.
        static void registerSaberCallback(const ThinVirtualLayer<void (void*, int, UnityEngine::Color)>& callback) {
            auto function = CondDep::Find<void, const ThinVirtualLayer<void (void*, int, UnityEngine::Color)>&>(CHROMA_ID, "registerSaberCallbackSafe");

            if (function) {
                function.value()(callback);
            }
        }
    };
}