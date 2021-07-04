#pragma once

#include "UnityEngine/Color.hpp"
#include "GlobalNamespace/SaberType.hpp"
#include "GlobalNamespace/SaberModelContainer.hpp"
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
        static std::optional<UnityEngine::Color> getGlobalSaberColorSafe(int saberType) noexcept {
            static auto function = CondDep::Find<OptColor, int>(CHROMA_ID, "getGlobalSaberColorSafe");

            if (function) {
                // Returns the color struct
                auto optColor = function.value()(saberType);

                if (!optColor.isSet) return std::nullopt;

                return optColor.getColor();
            }

            return std::nullopt;
        }

        /// Sets the saber color if the method was found.
        static void setGlobalSaberColorSafe(int saberType, std::optional<UnityEngine::Color> color) noexcept {
            static auto function = CondDep::Find<void, int, std::optional<UnityEngine::Color>>(CHROMA_ID, "setGlobalSaberColorSafe");

            if (function) {
                function.value()(saberType, color);
            }
        }

        /// Gets the saber color or null if either Chroma is not setting the color or method was not found
        static std::optional<UnityEngine::Color> getSaberColorSafe(GlobalNamespace::SaberModelController* saberModelController) noexcept {
            static auto function = CondDep::Find<OptColor, GlobalNamespace::SaberModelController*>(CHROMA_ID, "getSaberColorSafe");

            if (function) {
                // Returns the color struct
                auto optColor = function.value()(saberModelController);

                if (!optColor.isSet) return std::nullopt;

                return optColor.getColor();
            }

            return std::nullopt;
        }

        /// Sets the saber color if the method was found.
        static void setSaberColorSafe(GlobalNamespace::SaberModelController* saberModelController, std::optional<UnityEngine::Color> color) noexcept {
            static auto function = CondDep::Find<void, GlobalNamespace::SaberModelController*, std::optional<UnityEngine::Color>>(CHROMA_ID, "setSaberColorSafe");

            if (function) {
                function.value()(saberModelController, color);
            }
        }

        using ColorPair = std::pair<std::optional<UnityEngine::Color>, std::optional<UnityEngine::Color>>;
        using ColorOptPair = ExternPair<OptColor, OptColor>;
        /// Gets the saber color or null if either Chroma is not setting the color or method was not found
        static ColorPair getGlobalSabersColorSafe() noexcept {
            static auto function = CondDep::Find<ColorOptPair>(CHROMA_ID, "getGlobalSabersColorSafe");

            if (function) {
                // Returns the two rgba values
                auto pair = function.value()();

                return {pair.first.isSet ? std::make_optional(pair.first.getColor()) : std::nullopt,
                        pair.second.isSet ? std::make_optional(pair.second.getColor()) : std::nullopt};
            }

            return {std::nullopt, std::nullopt};
        }



        /// Sets if the saber is colorable. If this is set to true, Chroma will NOT color the saber and instead the work should be
        /// off loaded to getSaberChangedColorCallbackSafe
        /// Please reset this to false when you're no longer coloring it
        static void setSaberColorable(GlobalNamespace::SaberModelController* saberModelController, bool colorable) {
            static auto function = CondDep::Find<void, GlobalNamespace::SaberModelController*, bool>(CHROMA_ID, "setSaberColorable");

            if (function) {
                function.value()(saberModelController, colorable);
            }
        }

        /// Checks if the saber is colorable. If this is set to true, Chroma will NOT color the saber and instead the work should be
        /// off loaded to getSaberChangedColorCallbackSafe
        static std::optional<bool> isSaberColorable(GlobalNamespace::SaberModelController* saberModelController) {
            static auto function = CondDep::Find<bool, GlobalNamespace::SaberModelController*>(CHROMA_ID, "isSaberColorable");

            if (function) {
                return function.value()(saberModelController);
            }

            return std::nullopt;
        }

        /// This retrieves the callback used for Saber Color changed.
        ///
        static std::optional<std::reference_wrapper<UnorderedEventCallback<int, GlobalNamespace::SaberModelController*, UnityEngine::Color>>> getSaberChangedColorCallbackSafe() {
            static auto function = CondDep::Find<UnorderedEventCallback<int, GlobalNamespace::SaberModelController*, UnityEngine::Color>*>(CHROMA_ID, "getSaberChangedColorCallbackSafe");

            /// Oh boi what have I done
            if (function) {
                UnorderedEventCallback<int, GlobalNamespace::SaberModelController*, UnityEngine::Color>& callback = *function.value()();
                return std::make_optional(std::ref(callback));
            }

            return std::nullopt;
        }

        /// This registers a callback that is called whenever the saber color changes
        /// Do note however that every time a scene changes the callback is erased.
        static void registerSaberCallback(const ThinVirtualLayer<void (void*, int, GlobalNamespace::SaberModelController*, UnityEngine::Color)>& callback) {
            static auto function = CondDep::Find<void, const ThinVirtualLayer<void (void*, int, GlobalNamespace::SaberModelController*, UnityEngine::Color)>&>(CHROMA_ID, "registerSaberCallbackSafe");

            if (function) {
                function.value()(callback);
            }
        }
    };
}