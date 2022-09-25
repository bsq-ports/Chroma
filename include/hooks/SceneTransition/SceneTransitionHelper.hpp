#pragma once

#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/OverrideEnvironmentSettings.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"
#include "custom-json-data/shared/CustomBeatmapSaveDatav3.h"

namespace Chroma {
    class SceneTransitionHelper {
    public:
        static void Patch(GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap);
        static void Patch(GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap, GlobalNamespace::OverrideEnvironmentSettings*& overrideEnvironmentSettings);
        static bool BasicPatch(GlobalNamespace::IDifficultyBeatmap* customBeatmapDifficultyData, CustomJSONData::v3::CustomBeatmapSaveData* customBeatmapDataCustom);
    };
}