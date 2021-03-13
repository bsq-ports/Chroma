#pragma once

#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/OverrideEnvironmentSettings.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"

namespace Chroma {
    class SceneTransitionHelper {
    public:
        static void Patch(CustomJSONData::CustomBeatmapData* difficultyBeatmap);
        static void Patch(CustomJSONData::CustomBeatmapData* difficultyBeatmap, GlobalNamespace::OverrideEnvironmentSettings*& overrideEnvironmentSettings);
        static bool BasicPatch(GlobalNamespace::BeatmapData* customBeatmapData);
    };
}