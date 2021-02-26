#pragma once

#include "GlobalNamespace/BeatmapEventType.hpp"
#include "GlobalNamespace/BeatmapEventData.hpp"

#include "UnityEngine/Color.hpp"

#include "System/Collections/Generic/List_1.hpp"
#include "System/Collections/Generic/IReadOnlyList_1.hpp"

#include <unordered_map>
#include <vector>
#include <optional>
#include <utility>

namespace Chroma {
// Please let me delete this whole class
    class LegacyLightHelper {
    public:
        //internal
        inline const static int RGB_INT_OFFSET = 2000000000;

        // internal
        inline static std::unordered_map<int, std::vector<pair<float, UnityEngine::Color>>> LegacyColorEvents;

        // internal
        static void Activate(const std::vector<GlobalNamespace::BeatmapEventData*>& eventData);

        // internal
        static std::optional<UnityEngine::Color> GetLegacyColor(GlobalNamespace::BeatmapEventData* beatmapEventData);

    private:
        static UnityEngine::Color ColorFromInt(int rgb);
    };
}