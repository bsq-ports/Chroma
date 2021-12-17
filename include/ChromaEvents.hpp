#pragma once

#include "main.hpp"

#include "tracks/shared/AssociatedData.h"
#include "tracks/shared/Animation/Track.h"
#include "custom-json-data/shared/CustomEventData.h"
#include "custom-json-data/shared/CustomBeatmapData.h"

// Custom events, not Beatmap Events
namespace ChromaEvents {
    struct CustomEventAssociatedData {
        Track* track;
        bool parsed = false;
    };

    static std::unordered_map<CustomJSONData::CustomEventData const*, CustomEventAssociatedData> eventDataMap;

    static CustomEventAssociatedData &getEventAD(CustomJSONData::CustomEventData const* customData) {
        return eventDataMap[customData];
    }

    void deserialize(GlobalNamespace::IReadonlyBeatmapData* customBeatmapData);

    void parseEventData(TracksAD::BeatmapAssociatedData &beatmapAD, CustomJSONData::CustomEventData const* customData);

    void AddEventCallbacks(Logger &logger);
}
