#pragma once

#include "custom-json-data/shared/CustomBeatmapData.h"

namespace Chroma {
    class LegacyEnvironmentRemoval {
    public:
        static void Init(CustomJSONData::CustomBeatmapData* customBeatmap);
    };
}

