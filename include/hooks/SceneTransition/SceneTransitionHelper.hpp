#pragma once

#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/BeatmapLevelSO.hpp"
#include "GlobalNamespace/BeatmapLevelSO_DifficultyBeatmap.hpp"
#include "GlobalNamespace/OverrideEnvironmentSettings.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"

namespace Chroma {
    class SceneTransitionHelper {
    public:
        static void Patch(GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap);
        static void Patch(GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap, GlobalNamespace::OverrideEnvironmentSettings*& overrideEnvironmentSettings);
        static bool BasicPatch(GlobalNamespace::IDifficultyBeatmap* customBeatmapData, CustomJSONData::CustomBeatmapData* customBeatmapDataCustom);
    };
}