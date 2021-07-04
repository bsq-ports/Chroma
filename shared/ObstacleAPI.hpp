#pragma once

#include "UnityEngine/Color.hpp"
#include "conditional-dependencies/shared/main.hpp"
#include "utils.hpp"

#include "GlobalNamespace/ObstacleController.hpp"

#include <optional>

namespace Chroma {

    /// TODO: NOT TESTED, USE WITH CAUTION
    /// Uses conditional dependency to call the method safely or return
    /// nullopt if it isn't found or if the original method returned nullopt
    class ObstacleAPI {
    public:

        /// TODO: Unsure of whether this returns nullopt if Chroma sets the color or not.
        /// Gets the obstacle color or null if either Chroma is not setting the color or method was not found
        static std::optional<UnityEngine::Color> getObstacleControllerColorSafe(GlobalNamespace::ObstacleController *oc) noexcept {
            static auto function = CondDep::Find<OptColor, GlobalNamespace::ObstacleController*>(CHROMA_ID, "getObstacleControllerColorSafe");

            if (function) {
                // Returns the color struct
                auto optColor = function.value()(oc);

                if (!optColor.isSet) return std::nullopt;

                return optColor.getColor();
            }

            return std::nullopt;
        }

        /// Sets the obstacle color if the method was found.
        static void setObstacleColorSafe(GlobalNamespace::ObstacleController* nc, UnityEngine::Color color0) noexcept {
            static auto function = CondDep::Find<void, GlobalNamespace::ObstacleController*, UnityEngine::Color>(CHROMA_ID, "setObstacleColorSafe");

            if (function) {
                function.value()(nc, color0);
            }
        }

        /// Sets all the obstacle color if the method was found.
        static void setAllObstacleColorSafe(UnityEngine::Color color0) noexcept {
            static auto function = CondDep::Find<void, UnityEngine::Color>(CHROMA_ID, "setAllObstacleColorSafe");

            if (function) {
                function.value()(color0);
            }
        }

        /// Sets if the obstacle is colorable. If this is set to true, Chroma will NOT color the obstacle and
        /// instead the work should be done by the mod handling coloring obstacles usually a custom obstacles mod
        static void setObstacleColorable(bool colorable) {
            static auto function = CondDep::Find<void, bool>(CHROMA_ID, "setSaberColorable");

            if (function) {
                function.value()(colorable);
            }
        }

        /// Checks if the obstacle is colorable. If this is set to true, Chroma will NOT color the obstacle and
        /// instead the work should be done by the mod handling coloring obstacles usually a custom obstacle mod
        static std::optional<bool> isObstacleColorable() {
            static auto function = CondDep::Find<bool>(CHROMA_ID, "isSaberColorable");

            if (function) {
                return function.value()();
            }

            return std::nullopt;
        }
    };
}