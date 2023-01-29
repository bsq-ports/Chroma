#include "ChromaEvents.hpp"

#include "Chroma.hpp"
#include "lighting/ChromaFogController.hpp"

#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/BeatmapCallbacksController.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "custom-json-data/shared/CustomEventData.h"

#include "tracks/shared/TimeSourceHelper.h"
#include "tracks/shared/Vector.h"

using namespace GlobalNamespace;
using namespace NEVector;

void ChromaEvents::parseEventData(TracksAD::BeatmapAssociatedData &beatmapAD,
                                  CustomJSONData::CustomEventData const *customEventData, bool v2) {
    bool isType = false;

    auto typeHash = customEventData->typeHash;

#define TYPE_GET(jsonName, varName)                                \
    static auto jsonNameHash_##varName = std::hash<std::string_view>()(jsonName); \
    if (!isType && typeHash == (jsonNameHash_##varName))                      \
        isType = true;

    TYPE_GET(Chroma::OldConstants::ASSIGNFOGTRACK, ASSIGNFOGTRACK)

    if (!isType) {
        return;
    }

    rapidjson::Value const& eventData = *customEventData->data;
    auto& eventAD = getEventAD(customEventData);

    if (eventAD.parsed)
        return;

    eventAD.parsed = true;

    auto trackIt = eventData.FindMember((v2 ? Chroma::NewConstants::V2_TRACK : Chroma::NewConstants::TRACK).data());

    if (trackIt == eventData.MemberEnd() || trackIt->value.IsNull() || !trackIt->value.IsString()) {
        getLogger().debug("Track data is missing for Chroma custom event %f", customEventData->time);
        return;
    }

    std::string trackName(trackIt->value.GetString());
    Track *track = &beatmapAD.tracks.try_emplace(trackName, v2).first->second;

    eventAD.track = track;

    if (typeHash == jsonNameHash_ASSIGNFOGTRACK) {}
}

void ChromaEvents::deserialize(CustomJSONData::CustomBeatmapData *readOnlyBeatmap) {
    auto beatmapCast = readOnlyBeatmap;

    auto beatmap = beatmapCast;
    auto &beatmapAD = TracksAD::getBeatmapAD(beatmap->customData);

    if (!beatmapAD.valid) {
        TracksAD::readBeatmapDataAD(beatmap);
    }

    // Parse events
    for (auto const &customEventData: beatmap->customEventDatas) {
        if (!customEventData) continue;

        parseEventData(beatmapAD, customEventData, beatmap->v2orEarlier);
    }
}

void CustomEventCallback(BeatmapCallbacksController *callbackController,
                         CustomJSONData::CustomEventData *customEventData) {
    PAPER_IL2CPP_CATCH_HANDLER(
    bool isType = false;

    auto typeHash = customEventData->typeHash;

#define TYPE_GET(jsonName, varName)                                \
    static auto jsonNameHash_##varName = std::hash<std::string_view>()(jsonName); \
    if (!isType && typeHash == (jsonNameHash_##varName))                      \
        isType = true;

    TYPE_GET(Chroma::OldConstants::ASSIGNFOGTRACK, ASSIGNFOGTRACK)

    if (!isType) {
        return;
    }

    auto const &ad = ChromaEvents::getEventAD(customEventData);

    // fail safe, idek why this needs to be done smh
    // CJD you bugger
    if (!ad.parsed) {
        auto *customBeatmapData = (CustomJSONData::CustomBeatmapData *)callbackController->beatmapData;
        TracksAD::BeatmapAssociatedData &beatmapAD = TracksAD::getBeatmapAD(customBeatmapData->customData);
        ChromaEvents::parseEventData(beatmapAD, customEventData, customBeatmapData->v2orEarlier);
    }

    if (typeHash == jsonNameHash_ASSIGNFOGTRACK) {
        Chroma::ChromaFogController::getInstance()->AssignTrack(ad.track);
        CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Assigned fog controller to track");
    }
    )
}

void ChromaEvents::AddEventCallbacks(Logger &logger) {
    CustomJSONData::CustomEventCallbacks::AddCustomEventCallback(&CustomEventCallback);
}
