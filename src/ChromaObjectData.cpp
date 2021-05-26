#include "ChromaObjectData.hpp"

#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/BeatmapLineData.hpp"
#include "GlobalNamespace/BeatmapObjectData.hpp"
#include "System/Collections/Generic/List_1.hpp"
#include "System/Collections/Generic/IReadOnlyList_1.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"

#include "utils/ChromaUtils.hpp"

using namespace ChromaUtils;


void Chroma::ChromaObjectDataManager::deserialize(GlobalNamespace::IReadonlyBeatmapData *beatmapData) {
    ChromaObjectDatas.clear();

    static auto contextLogger = getLogger().WithContext(ChromaLogger::ObjectDataDeserialize);

    auto beatmapDataCast = il2cpp_utils::cast<GlobalNamespace::BeatmapData>(beatmapData);
    auto beatmapLines = il2cpp_utils::cast<Array<GlobalNamespace::IReadonlyBeatmapLineData*>>(beatmapDataCast->get_beatmapLinesData());

    ChromaObjectDatas.reserve(beatmapLines->Length());

    debugSpamLog(contextLogger, "Array klass: %s", il2cpp_utils::ClassStandardName(beatmapLines->klass).c_str());

    for (int i = 0; i < beatmapLines->Length(); i++) {
        auto beatmapLineData = il2cpp_utils::cast<GlobalNamespace::BeatmapLineData>(beatmapLines->get(i));

        if (beatmapLineData && beatmapLineData->beatmapObjectsData) {
            debugSpamLog(contextLogger, "Klass: %s", il2cpp_utils::ClassStandardName(beatmapLineData->klass).c_str());
            for (int j = 0; j < beatmapLineData->beatmapObjectsData->items->Length(); j++) {
                auto beatmapObjectData = beatmapLineData->beatmapObjectsData->items->get(j);

                if (beatmapObjectData) {
                    std::shared_ptr<ChromaObjectData> chromaObjectData;

                    std::optional<std::reference_wrapper<rapidjson::Value>> dynData;

                    static auto CustomNoteDataKlass = classof(CustomJSONData::CustomNoteData *);
                    static auto CustomObstacleDataKlass = classof(CustomJSONData::CustomObstacleData *);
                    static auto CustomWaypointDataKlass = classof(CustomJSONData::CustomWaypointData *);

                    if (ASSIGNMENT_CHECK(CustomNoteDataKlass, beatmapObjectData->klass)) {
                        debugSpamLog(contextLogger, "Custom note %s", il2cpp_utils::ClassStandardName(beatmapObjectData->klass).c_str());
                        auto *customBeatmapEvent = il2cpp_utils::cast<CustomJSONData::CustomNoteData>(beatmapObjectData);

                        dynData = customBeatmapEvent->customData->value;

                        auto data = std::make_shared<ChromaNoteData>();
                        debugSpamLog(contextLogger, "Color");
                        data->Color = ChromaUtilities::GetColorFromData(dynData);
                        if (dynData)
                            PrintJSONValue(dynData.value());
                        debugSpamLog(contextLogger, "Spawn effects");

                        data->DisableSpawnEffect = getIfExists<bool>(dynData, DISABLESPAWNEFFECT);

                        debugSpamLog(contextLogger,"Shared ptr custom note");
                        chromaObjectData = data;
                    } else if (ASSIGNMENT_CHECK(CustomObstacleDataKlass,beatmapObjectData->klass)) {
                        debugSpamLog(contextLogger, "Custom obstacle");
                        auto *customBeatmapEvent = il2cpp_utils::cast<CustomJSONData::CustomObstacleData>(beatmapObjectData);

                        dynData = customBeatmapEvent->customData->value;

                        auto data = std::make_shared<ChromaObjectData>();

                        data->Color = ChromaUtilities::GetColorFromData(dynData);

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

                    // TODO: Do Noodle stuff here
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
