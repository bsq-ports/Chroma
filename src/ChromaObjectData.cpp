#include "ChromaObjectData.hpp"

#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/BeatmapLineData.hpp"
#include "GlobalNamespace/BeatmapObjectData.hpp"
#include "System/Collections/Generic/List_1.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"

#include "utils/ChromaUtils.hpp"

using namespace ChromaUtils;


void Chroma::ChromaObjectDataManager::deserialize(GlobalNamespace::IReadonlyBeatmapData *beatmapData) {
    ChromaObjectDatas.clear();

    auto beatmapDataCast = reinterpret_cast<GlobalNamespace::BeatmapData*>(beatmapData);
    auto beatmapLines = reinterpret_cast<System::Collections::Generic::List_1<GlobalNamespace::IReadonlyBeatmapLineData*>*>(beatmapDataCast->get_beatmapLinesData());

    for (int i = 0; i < beatmapLines->items->Length(); i++) {
        auto beatmapLineData = reinterpret_cast<GlobalNamespace::BeatmapLineData *>(beatmapLines->items->values[i]);
        for (int j = 0; j < beatmapLineData->beatmapObjectsData->items->Length(); j++) {
            auto beatmapObjectData = beatmapLineData->beatmapObjectsData->items->values[j];

            std::shared_ptr<ChromaObjectData> chromaObjectData;

            if (beatmapObjectData) {
                rapidjson::Value* dynData;

                if (ASSIGNMENT_CHECK(classof(CustomJSONData::CustomNoteData*),beatmapObjectData->klass)) {
                    auto *customBeatmapEvent = reinterpret_cast<CustomJSONData::CustomNoteData *>(beatmapObjectData);

                    bool isCustomData = customBeatmapEvent->customData && customBeatmapEvent->customData->value &&
                                        customBeatmapEvent->customData->value->IsObject();
                    dynData = isCustomData ? customBeatmapEvent->customData->value : nullptr;

                    auto* data = new ChromaNoteData();
                    data->Color = ChromaUtilities::GetColorFromData(dynData);
                    data->DisableSpawnEffect = getIfExists<bool>(dynData, DISABLESPAWNEFFECT);

                    chromaObjectData = std::shared_ptr<ChromaObjectData>(data);
                } else if (ASSIGNMENT_CHECK(classof(CustomJSONData::CustomObstacleData*), beatmapObjectData->klass)) {
                    auto *customBeatmapEvent = reinterpret_cast<CustomJSONData::CustomObstacleData*>(beatmapObjectData);

                    bool isCustomData = customBeatmapEvent->customData && customBeatmapEvent->customData->value &&
                                        customBeatmapEvent->customData->value->IsObject();
                    dynData = isCustomData ? customBeatmapEvent->customData->value : nullptr;

                    auto* data = new ChromaObjectData();
                    data->Color = ChromaUtilities::GetColorFromData(dynData);

                    chromaObjectData = std::shared_ptr<ChromaObjectData>(data);
                } else if (ASSIGNMENT_CHECK(classof(CustomJSONData::CustomWaypointData*), beatmapObjectData->klass)) {
                    auto *customBeatmapEvent = reinterpret_cast<CustomJSONData::CustomWaypointData*>(beatmapObjectData);

                    // TODO: uncomment when CJD adds customData
//                    bool isCustomData = customBeatmapEvent->customData && customBeatmapEvent->customData->value &&
//                                        customBeatmapEvent->customData->value->IsObject();
//                    dynData = isCustomData ? customBeatmapEvent->customData->value : nullptr;

                    auto* data = new ChromaObjectData();

                    chromaObjectData = std::shared_ptr<ChromaObjectData>(data);
                } else continue;

                // TODO: Do Noodle stuff here

                ChromaObjectDatas[beatmapObjectData] = chromaObjectData;
            }
        }
    }
}
