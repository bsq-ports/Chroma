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

    auto beatmapDataCast = reinterpret_cast<GlobalNamespace::BeatmapData*>(beatmapData);
    auto beatmapLines = reinterpret_cast<Array<GlobalNamespace::IReadonlyBeatmapLineData*>*>(beatmapDataCast->get_beatmapLinesData());

    debugSpamLog(contextLogger, "Array klass: %s", il2cpp_utils::ClassStandardName(beatmapLines->klass).c_str());

    for (int i = 0; i < beatmapLines->Length(); i++) {
        auto beatmapLineData = reinterpret_cast<GlobalNamespace::BeatmapLineData *>(beatmapLines->values[i]);

        if (beatmapLineData && beatmapLineData->beatmapObjectsData) {
            debugSpamLog(contextLogger, "Klass: %s", il2cpp_utils::ClassStandardName(beatmapLineData->klass).c_str());
            for (int j = 0; j < beatmapLineData->beatmapObjectsData->items->Length(); j++) {
                auto beatmapObjectData = beatmapLineData->beatmapObjectsData->items->values[j];

                if (beatmapObjectData) {
                    std::shared_ptr<ChromaObjectData> chromaObjectData(nullptr);

                    std::optional<std::reference_wrapper<rapidjson::Value>> dynData = std::nullopt;

                    if (ASSIGNMENT_CHECK(classof(CustomJSONData::CustomNoteData * ), beatmapObjectData->klass)) {
                        debugSpamLog(contextLogger, "Custom note %s", il2cpp_utils::ClassStandardName(beatmapObjectData->klass).c_str());
                        auto *customBeatmapEvent = reinterpret_cast<CustomJSONData::CustomNoteData *>(beatmapObjectData);

                        dynData = customBeatmapEvent->customData->value;

                        auto *data = new ChromaNoteData();
                        debugSpamLog(contextLogger, "Color");
                        data->Color = ChromaUtilities::GetColorFromData(dynData);
                        debugSpamLog(contextLogger, "Spawn effects");

                        data->DisableSpawnEffect = getIfExists<bool>(dynData, DISABLESPAWNEFFECT);

                        debugSpamLog(contextLogger,"Shared ptr custom note");
                        chromaObjectData = std::shared_ptr<ChromaNoteData>(data);
                    } else if (ASSIGNMENT_CHECK(classof(CustomJSONData::CustomObstacleData * ),
                                                beatmapObjectData->klass)) {
                        debugSpamLog(contextLogger, "Custom obstacle");
                        auto *customBeatmapEvent = reinterpret_cast<CustomJSONData::CustomObstacleData *>(beatmapObjectData);

                        dynData = customBeatmapEvent->customData->value;

                        auto *data = new ChromaObjectData();

                        data->Color = ChromaUtilities::GetColorFromData(dynData);

                        chromaObjectData = std::shared_ptr<ChromaObjectData>(data);
                    } else if (ASSIGNMENT_CHECK(classof(CustomJSONData::CustomWaypointData * ),
                                                beatmapObjectData->klass)) {
                        debugSpamLog(contextLogger, "Custom waypoint");
                        auto *customBeatmapEvent = reinterpret_cast<CustomJSONData::CustomWaypointData *>(beatmapObjectData);

                        // TODO: uncomment when CJD adds customData
//                    bool isCustomData = customBeatmapEvent->customData && customBeatmapEvent->customData->value &&
//                                        customBeatmapEvent->customData->value->IsObject();
//                    dynData = isCustomData ? customBeatmapEvent->customData->value : nullptr;

                        auto *data = new ChromaObjectData();

                        data->Color = std::nullopt;


                        chromaObjectData = std::shared_ptr<ChromaObjectData>(data);
                    } else continue;

                    // TODO: Do Noodle stuff here
                    debugSpamLog(contextLogger,"Adding to list");
                    ChromaObjectDatas[beatmapObjectData] = chromaObjectData;
                    debugSpamLog(contextLogger,"Added to list");
                }
            }
        }
    }
}
