#include "ChromaObjectData.hpp"

#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/BeatmapLineData.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"

#include "tracks/shared/Animation/Animation.h"

#include "utils/ChromaUtils.hpp"

using namespace ChromaUtils;


void Chroma::ChromaObjectDataManager::deserialize(GlobalNamespace::IReadonlyBeatmapData *beatmapData) {
    ChromaObjectDatas.clear();

    static auto contextLogger = getLogger().WithContext(ChromaLogger::ObjectDataDeserialize);

    auto beatmapDataCast = il2cpp_utils::cast<CustomJSONData::CustomBeatmapData>(beatmapData);
    auto beatmapLines = il2cpp_utils::cast<Array<GlobalNamespace::IReadonlyBeatmapLineData*>>(beatmapDataCast->get_beatmapLinesData());

    debugSpamLog(contextLogger, "Array klass: %s", il2cpp_utils::ClassStandardName(beatmapLines->klass).c_str());

    TracksAD::BeatmapAssociatedData& beatmapAD = TracksAD::getBeatmapAD(beatmapDataCast->customData);


    for (int i = 0; i < beatmapLines->Length(); i++) {
        auto beatmapLineData = il2cpp_utils::cast<GlobalNamespace::BeatmapLineData>(beatmapLines->get(i));

        if (beatmapLineData && beatmapLineData->beatmapObjectsData) {
            for (int j = 0; j < beatmapLineData->beatmapObjectsData->items->Length(); j++) {
                auto beatmapObjectData = beatmapLineData->beatmapObjectsData->items->get(j);

                if (beatmapObjectData) {
                    std::shared_ptr<ChromaObjectData> chromaObjectData;

                    CustomJSONData::JSONWrapper* eventDynData = nullptr;

                    static auto CustomNoteDataKlass = classof(CustomJSONData::CustomNoteData *);
                    static auto CustomObstacleDataKlass = classof(CustomJSONData::CustomObstacleData *);
                    static auto CustomWaypointDataKlass = classof(CustomJSONData::CustomWaypointData *);

                    if (ASSIGNMENT_CHECK(CustomNoteDataKlass, beatmapObjectData->klass)) {
                        debugSpamLog(contextLogger, "Custom note %s", il2cpp_utils::ClassStandardName(beatmapObjectData->klass).c_str());
                        auto *customBeatmapEvent = il2cpp_utils::cast<CustomJSONData::CustomNoteData>(beatmapObjectData);

                        eventDynData = customBeatmapEvent->customData;

                        auto data = std::make_shared<ChromaNoteData>();

                        data->Color = ChromaUtilities::GetColorFromData(eventDynData->value);
                        data->DisableSpawnEffect = getIfExists<bool>(eventDynData->value, DISABLESPAWNEFFECT);

                        chromaObjectData = data;
                    } else if (ASSIGNMENT_CHECK(CustomObstacleDataKlass,beatmapObjectData->klass)) {
                        debugSpamLog(contextLogger, "Custom obstacle");
                        auto *customBeatmapEvent = il2cpp_utils::cast<CustomJSONData::CustomObstacleData>(beatmapObjectData);

                        eventDynData = customBeatmapEvent->customData;

                        auto data = std::make_shared<ChromaObjectData>();

                        data->Color = ChromaUtilities::GetColorFromData(eventDynData->value);

                        chromaObjectData = data;
                    } else if (ASSIGNMENT_CHECK(CustomWaypointDataKlass,beatmapObjectData->klass)) {
                        debugSpamLog(contextLogger, "Custom waypoint");
                        auto *customBeatmapEvent = il2cpp_utils::cast<CustomJSONData::CustomWaypointData>(beatmapObjectData);

                        // TODO: uncomment when CJD adds customData
                        // bool isCustomData = customBeatmapEvent->customData && customBeatmapEvent->customData->value &&
                        //                customBeatmapEvent->customData->value->IsObject();
                        // dynData = isCustomData ? customBeatmapEvent->customData->value : nullptr;

                        auto data = std::make_shared<ChromaObjectData>();

                        data->Color = std::nullopt;


                        chromaObjectData = data;
                    } else continue;

                    if (chromaObjectData && beatmapDataCast->customData && eventDynData->value) {

                        rapidjson::Value& customData = *eventDynData->value;
                        auto animationObjectDyn = customData.FindMember(Chroma::ANIMATION);
                        if (animationObjectDyn != customData.MemberEnd()) {
                            PointDefinition *anonPointDef = nullptr;
                            PointDefinition *localColor = Animation::TryGetPointData(beatmapAD, anonPointDef,
                                                                                     animationObjectDyn->value, Chroma::COLOR);

                            if (anonPointDef) {
                                beatmapAD.anonPointDefinitions.push_back(anonPointDef);
                            }

                            chromaObjectData->LocalPathColor = localColor ? std::make_optional(localColor)
                                                                          : std::nullopt;
                        }

                        auto& tracks = TracksAD::getAD(eventDynData).tracks;
                        chromaObjectData->Tracks = tracks;
                    }
                    debugSpamLog(contextLogger,"Adding to list");
                    ChromaObjectDatas[beatmapObjectData] = chromaObjectData;
                    debugSpamLog(contextLogger,"Added to list");
                }
            }
        }
    }
}
