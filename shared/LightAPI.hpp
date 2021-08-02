#pragma once

#include "UnityEngine/Color.hpp"
#include "conditional-dependencies/shared/main.hpp"
#include "utils.hpp"

#include "GlobalNamespace/LightSwitchEventEffect.hpp"
#include "GlobalNamespace/ILightWithId.hpp"
#include "GlobalNamespace/BeatmapEventType.hpp"

#include <unordered_map>
#include <optional>
#include <vector>
#include <memory>

namespace Chroma {

    /// TODO: NOT TESTED, USE WITH CAUTION
    /// Uses conditional dependency to call the method safely or return
    /// nullopt if it isn't found or if the original method returned nullopt
    class LightAPI {
    public:

        /// If a value is std::nullopt, it likely is to be ignored or used as a reset switch.
        /// In other words, if you make it nullopt, it will consider that as a reset method.
        struct LSEData {
            std::optional<Sombrero::FastColor> _lightColor0;
            std::optional<Sombrero::FastColor> _lightColor1;
            std::optional<Sombrero::FastColor> _lightColor0Boost = std::nullopt;
            std::optional<Sombrero::FastColor> _lightColor1Boost = std::nullopt;
        };

        /// Gets the lights
        ///
        /// Note this is particularly expensive since
        /// it is creating a ptr copy of the list on the internal side
        /// then moving it back to a value and deleting it in the API side
        ///
        /// Avoid using this in a loop or frequent method,
        /// you can cache this during a song perhaps?
        static std::optional<std::unordered_map<int, GlobalNamespace::ILightWithId*>> getLightsSafe(GlobalNamespace::LightSwitchEventEffect *lse) {
            static auto function = CondDep::Find<std::unordered_map<int, GlobalNamespace::ILightWithId*>*, GlobalNamespace::LightSwitchEventEffect*>(CHROMA_ID, "getLightsSafe");

            if (function) {
                auto val = function.value()(lse);

                std::unordered_map<int, GlobalNamespace::ILightWithId*> vec(std::move(*val));

                delete val;

                return vec;
            }

            return std::nullopt;
        }

        /// Gets the light prop grouped
        ///
        /// Note this is particularly expensive since
        /// it is creating a ptr copy of the list on the internal side
        /// then moving it back to a value and deleting it in the API side
        ///
        /// Avoid using this in a loop or frequent method,
        /// you can cache this during a song perhaps?
        static std::optional<std::unordered_map<int, std::vector<GlobalNamespace::ILightWithId *>>>
        getLightsPropagationGroupedSafe(GlobalNamespace::LightSwitchEventEffect *lse) {
            static auto function = CondDep::Find<std::unordered_map<int, std::vector<GlobalNamespace::ILightWithId *>>*, GlobalNamespace::LightSwitchEventEffect*>(CHROMA_ID, "getLightsPropagationGroupedSafe");

            if (function) {
                auto val = function.value()(lse);

                // TODO: This is likely to crash
                std::unordered_map<int, std::vector<GlobalNamespace::ILightWithId *>> map(std::move(*val));

                delete val;

                return map;
            }

            return std::nullopt;
        }


        /// Gets the light color or null if method was not found or
        /// if Chroma is not managing the light
        //
        /// The LSE Data if it is found, none of the values will be std::nullopt.
        ///
        /// This is a bit slower than usual since it creates a heap struct, then copies to a value struct and finally deletes.
        static std::optional<LSEData> getLightColorSafe(GlobalNamespace::BeatmapEventType mb) noexcept {
            static auto function = CondDep::Find<LSEData*, GlobalNamespace::BeatmapEventType>(CHROMA_ID, "getLightColorSafe");

            if (function) {

                auto val = function.value()(mb);

                if (!val) return std::nullopt;

                LSEData dataCopy = *val;

                delete val;

                // Returns the rgba value or -1 if it was a nullopt
                return dataCopy;
            }

            return std::nullopt;
        }

        /// Sets the light color if the method was found.
        /// If nullopt, it resets the colors
        static void setLightColorSafe(GlobalNamespace::BeatmapEventType mb, bool refresh, std::optional<LSEData> color) noexcept {
            static auto function = CondDep::Find<void, GlobalNamespace::BeatmapEventType, bool, std::optional<LSEData>>(CHROMA_ID, "setLightColorSafe");

            if (function) {
                function.value()(mb, refresh, color);
            }
        }

        /// Sets all the light colors
        /// If nullopt, it resets all the light colors
        static void SetAllLightingColorsSafe(std::optional<LSEData> lseData) {
            static auto function = CondDep::Find<void, std::optional<LSEData>>(CHROMA_ID, "setAllLightingColorsSafe");

            if (function) {
                function.value()(lseData);
            }
        }

    };
}