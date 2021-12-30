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

    auto beatmapLinesLength = beatmapLines->Length();
    for (int i = 0; i < beatmapLinesLength; i++) {
        auto beatmapLineData = il2cpp_utils::cast<GlobalNamespace::BeatmapLineData>(beatmapLines->get(i));

        if (beatmapLineData && beatmapLineData->beatmapObjectsData) {
            for (auto beatmapObjectData : beatmapLineData->beatmapObjectsData->items) {


                if (!beatmapObjectData) continue;

                std::shared_ptr<ChromaObjectData> chromaObjectData;

                CustomJSONData::JSONWrapper* objectDynData = nullptr;

                static auto CustomNoteDataKlass = classof(CustomJSONData::CustomNoteData *);
                static auto CustomObstacleDataKlass = classof(CustomJSONData::CustomObstacleData *);
                static auto CustomWaypointDataKlass = classof(CustomJSONData::CustomWaypointData *);

                if (ASSIGNMENT_CHECK(CustomNoteDataKlass, beatmapObjectData->klass)) {
                    debugSpamLog(contextLogger, "Custom note %s", il2cpp_utils::ClassStandardName(beatmapObjectData->klass).c_str());
                    auto *customNoteData = static_cast<CustomJSONData::CustomNoteData *>(beatmapObjectData);

                    objectDynData = customNoteData->customData;

                    auto data = std::make_shared<ChromaNoteData>();

                    data->Color = ChromaUtilities::GetColorFromData(objectDynData->value);
                    data->DisableSpawnEffect = getIfExists<bool>(objectDynData->value, DISABLESPAWNEFFECT);

                    chromaObjectData = data;
                } else if (ASSIGNMENT_CHECK(CustomObstacleDataKlass,beatmapObjectData->klass)) {
                    debugSpamLog(contextLogger, "Custom obstacle");
                    auto *customObstacleData = static_cast<CustomJSONData::CustomObstacleData *>(beatmapObjectData);

                    objectDynData = customObstacleData->customData;

                    auto data = std::make_shared<ChromaObjectData>();

                    data->Color = ChromaUtilities::GetColorFromData(objectDynData->value);

                    chromaObjectData = data;
                }
//                else if (false && ASSIGNMENT_CHECK(CustomWaypointDataKlass,beatmapObjectData->klass)) {
//                    debugSpamLog(contextLogger, "Custom waypoint");
//                    auto *customBeatmapEvent = il2cpp_utils::cast<CustomJSONData::CustomWaypointData>(beatmapObjectData);
//
//                    // TODO: uncomment when CJD adds customData
//                    // bool isCustomData = customBeatmapEvent->customData && customBeatmapEvent->customData->value &&
//                    //                customBeatmapEvent->customData->value->IsObject();
//                    // dynData = isCustomData ? customBeatmapEvent->customData->value : nullptr;
//
//                    auto data = std::make_shared<ChromaObjectData>();
//
//                    data->Color = std::nullopt;
//
//
//                    chromaObjectData = data;
//                }
                else continue;

                if (chromaObjectData) {
                    if (objectDynData->value) {
                        rapidjson::Value &customData = *objectDynData->value;
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
                    }
                    auto &tracks = TracksAD::getAD(objectDynData).tracks;
                    chromaObjectData->Tracks = tracks;

                    debugSpamLog(contextLogger,"Adding to list");
                    ChromaObjectDatas[beatmapObjectData] = chromaObjectData;
                    debugSpamLog(contextLogger,"Added to list");
                }
            }
        }
    }
}
