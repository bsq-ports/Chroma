#pragma once

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "UnityEngine/MonoBehaviour.hpp"

namespace Chroma {
    class LightColorManager {
    public:
        static void ColorLightSwitch(GlobalNamespace::BeatmapEventData* beatmapEventData);
    };
}