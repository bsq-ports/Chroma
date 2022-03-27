#include "ChromaObjectData.hpp"

#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/BeatmapLineData.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"

#include "tracks/shared/Animation/Animation.h"

#include "utils/ChromaUtils.hpp"

using namespace ChromaUtils;
using namespace GlobalNamespace;


void Chroma::ChromaObjectDataManager::deserialize(GlobalNamespace::IReadonlyBeatmapData *beatmapData) {
    ChromaObjectDatas.clear();

    static auto contextLogger = getLogger().WithContext(ChromaLogger::ObjectDataDeserialize);

    auto beatmapDataCast = il2cpp_utils::cast<CustomJSONData::CustomBeatmapData>(beatmapData);
    auto notes = beatmapDataCast->GetBeatmapItemsCpp<NoteData *>();
    auto obstacles = beatmapDataCast->GetBeatmapItemsCpp<ObstacleData *>();

    std::vector<BeatmapObjectData *> objects(notes.size() + obstacles.size());

    std::copy(notes.begin(), notes.end(), std::back_inserter(objects));
    std::copy(obstacles.begin(), obstacles.end(), std::back_inserter(objects));


    debugSpamLog(contextLogger, "Array klass: %s", il2cpp_utils::ClassStandardName(beatmapLines->klass).c_str());

    TracksAD::BeatmapAssociatedData &beatmapAD = TracksAD::getBeatmapAD(beatmapDataCast->customData);


    for (auto beatmapObjectData: objects) {


        if (!beatmapObjectData) continue;

        ChromaObjectData chromaObjectData;

        CustomJSONData::JSONWrapper *objectDynData = nullptr;

        static auto CustomNoteDataKlass = classof(CustomJSONData::CustomNoteData *);
        static auto CustomObstacleDataKlass = classof(CustomJSONData::CustomObstacleData *);
        static auto CustomWaypointDataKlass = classof(CustomJSONData::CustomWaypointData *);

        if (ASSIGNMENT_CHECK(CustomNoteDataKlass, beatmapObjectData->klass)) {
            debugSpamLog(contextLogger, "Custom note %s",
                         il2cpp_utils::ClassStandardName(beatmapObjectData->klass).c_str());
            auto *customNoteData = static_cast<CustomJSONData::CustomNoteData *>(beatmapObjectData);

            objectDynData = customNoteData->customData;

            chromaObjectData.Color = ChromaUtilities::GetColorFromData(objectDynData->value);
            chromaObjectData.DisableSpawnEffect = getIfExists<bool>(objectDynData->value, DISABLESPAWNEFFECT);
        } else if (ASSIGNMENT_CHECK(CustomObstacleDataKlass, beatmapObjectData->klass)) {
            auto *customObstacleData = static_cast<CustomJSONData::CustomObstacleData *>(beatmapObjectData);

            objectDynData = customObstacleData->customData;
            chromaObjectData.Color = ChromaUtilities::GetColorFromData(objectDynData->value);
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


        if (objectDynData && objectDynData->value) {
            rapidjson::Value const &customData = *objectDynData->value;
            auto animationObjectDyn = customData.FindMember(Chroma::ANIMATION.data());
            if (animationObjectDyn != customData.MemberEnd()) {
                PointDefinition *anonPointDef = nullptr;
                PointDefinition *localColor = Animation::TryGetPointData(beatmapAD, anonPointDef,
                                                                         animationObjectDyn->value,
                                                                         Chroma::COLOR);

                if (anonPointDef) {
                    beatmapAD.anonPointDefinitions.push_back(anonPointDef);
                }

                chromaObjectData.LocalPathColor = localColor ? std::make_optional(localColor) : std::nullopt;
            }
        }
        auto &tracks = TracksAD::getAD(objectDynData).tracks;
        chromaObjectData.Tracks = tracks;

        ChromaObjectDatas.try_emplace(beatmapObjectData, std::move(chromaObjectData));
    }
}
