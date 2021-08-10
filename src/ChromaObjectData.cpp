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

    TracksAD::BeatmapAssociatedData& trackAD = TracksAD::getBeatmapAD(beatmapDataCast->customData);

    for (int i = 0; i < beatmapLines->Length(); i++) {
        auto beatmapLineData = il2cpp_utils::cast<GlobalNamespace::BeatmapLineData>(beatmapLines->get(i));

        if (beatmapLineData && beatmapLineData->beatmapObjectsData) {
            debugSpamLog(contextLogger, "Klass: %s", il2cpp_utils::ClassStandardName(beatmapLineData->klass).c_str());
            for (int j = 0; j < beatmapLineData->beatmapObjectsData->items->Length(); j++) {
                auto beatmapObjectData = beatmapLineData->beatmapObjectsData->items->get(j);

                if (beatmapObjectData) {
                    std::shared_ptr<ChromaObjectData> chromaObjectData;

                    CustomJSONData::JSONWrapper* eventDynData;

                    static auto CustomNoteDataKlass = classof(CustomJSONData::CustomNoteData *);
                    static auto CustomObstacleDataKlass = classof(CustomJSONData::CustomObstacleData *);
                    static auto CustomWaypointDataKlass = classof(CustomJSONData::CustomWaypointData *);

                    if (ASSIGNMENT_CHECK(CustomNoteDataKlass, beatmapObjectData->klass)) {
                        debugSpamLog(contextLogger, "Custom note %s", il2cpp_utils::ClassStandardName(beatmapObjectData->klass).c_str());
                        auto *customBeatmapEvent = il2cpp_utils::cast<CustomJSONData::CustomNoteData>(beatmapObjectData);

                        eventDynData = customBeatmapEvent->customData;

                        auto data = std::make_shared<ChromaNoteData>();
                        debugSpamLog(contextLogger, "Color");
                        data->Color = ChromaUtilities::GetColorFromData(eventDynData->value);
                        if (eventDynData->value)
                            PrintJSONValue(eventDynData->value.value());
                        debugSpamLog(contextLogger, "Spawn effects");

                        data->DisableSpawnEffect = getIfExists<bool>(eventDynData->value, DISABLESPAWNEFFECT);

                        debugSpamLog(contextLogger,"Shared ptr custom note");
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
                        rapidjson::Value& beatmapCustomData = *beatmapDataCast->customData->value;
                        rapidjson::Value& customData = *eventDynData->value;
                        auto animationObjectDyn = customData.FindMember(Chroma::ANIMATION);
                        if (animationObjectDyn != customData.MemberEnd())
                        {
                            auto pointDefinitions = beatmapCustomData.FindMember("pointDefinitions");

                            if (pointDefinitions != beatmapCustomData.MemberEnd()) {
                                PointDefinition* localColor = nullptr;



                                Animation::TryGetPointData(trackAD, localColor, pointDefinitions->value, Chroma::COLOR);

                                chromaObjectData->LocalPathColor = localColor ? std::make_optional(localColor) : std::nullopt;
                            }
                        }

                        auto track = TracksAD::getAD(eventDynData).track;
                        chromaObjectData->Track = track ? std::make_optional(track) : std::nullopt;
                    }
                    debugSpamLog(contextLogger,"Adding to list");
                    ChromaObjectDatas[beatmapObjectData] = chromaObjectData;
                    debugSpamLog(contextLogger,"Added to list");
                }
            }
        }
    }

    // Deallocate unused memory.
    auto shrinkedMap = ChromaObjectDataType(ChromaObjectDatas.begin(), ChromaObjectDatas.end());
    ChromaObjectDatas = shrinkedMap;
}
