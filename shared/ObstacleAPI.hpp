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
            auto function = CondDep::Find<int, GlobalNamespace::ObstacleController*>("chroma", "getObstacleControllerColorSafe");

            if (function) {
                // Returns the rgba value or -1 if it was a nullopt
                auto rgba = function.value()(oc);

                if (rgba == -1) return std::nullopt;

                auto color = ColourManager::ColourFromInt(rgba);

                return std::make_optional(color);
            }

            return std::nullopt;
        }

        /// Sets the obstacle color if the method was found.
        static void setObstacleColorSafe(GlobalNamespace::ObstacleController* nc, UnityEngine::Color color0) noexcept {
            auto function = CondDep::Find<void, GlobalNamespace::ObstacleController*, UnityEngine::Color>("chroma", "setObstacleColorSafe");

            if (function) {
                function.value()(nc, color0);
            }
        }

        /// Sets all the obstacle color if the method was found.
        static void setAllObstacleColorSafe(UnityEngine::Color color0) noexcept {
            auto function = CondDep::Find<void, UnityEngine::Color>("chroma", "setAllObstacleColorSafe");

            if (function) {
                function.value()(color0);
            }
        }
    };
}