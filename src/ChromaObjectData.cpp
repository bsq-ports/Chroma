#include "ChromaObjectData.hpp"

// Third-party includes
#include "custom-json-data/shared/CustomBeatmapData.h"
#include "tracks/shared/Animation/Animation.h"
#include "tracks/shared/AssociatedData.h"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

// Game includes
#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/BeatmapLineData.hpp"
#include "GlobalNamespace/BeatmapObjectData.hpp"

// Project includes
#include "utils/ChromaUtils.hpp"
#include "main.hpp"   // For debugSpamLog
#include "Chroma.hpp" // For NewConstants namespace

using namespace ChromaUtils;
using namespace GlobalNamespace;
using namespace TracksAD::Constants;

using namespace ChromaUtils;
using namespace GlobalNamespace;

void Chroma::ChromaObjectDataManager::deserialize(CustomJSONData::CustomBeatmapData* beatmapData) {
  ChromaObjectDatas.clear();

  auto* beatmapDataCast = beatmapData;
  bool v2 = beatmapDataCast->v2orEarlier;

  debugSpamLog("Array klass: {}", il2cpp_utils::ClassStandardName(beatmapLines->klass).c_str());

  TracksAD::BeatmapAssociatedData& beatmapAD = TracksAD::getBeatmapAD(beatmapDataCast->customData);

  static auto* CustomNoteDataKlass = classof(CustomJSONData::CustomNoteData*);
  static auto* CustomSliderDataKlass = classof(CustomJSONData::CustomSliderData*);
  static auto* CustomObstacleDataKlass = classof(CustomJSONData::CustomObstacleData*);
  static auto* CustomWaypointDataKlass = classof(CustomJSONData::CustomWaypointData*);

  for (auto* beatmapObjectData : beatmapData->beatmapObjectDatas) {
    if (beatmapObjectData == nullptr) {
      continue;
    }

    ChromaObjectData chromaObjectData;

    CustomJSONData::JSONWrapper* objectDynData = nullptr;

    if (ASSIGNMENT_CHECK(CustomNoteDataKlass, beatmapObjectData->klass)) {
      debugSpamLog("Custom note {}", il2cpp_utils::ClassStandardName(beatmapObjectData->klass).c_str());
      auto* customNoteData = reinterpret_cast<CustomJSONData::CustomNoteData*>(beatmapObjectData);

      objectDynData = customNoteData->customData;

      chromaObjectData.Color = ChromaUtilities::GetColorFromData(objectDynData->value, v2);
      chromaObjectData.SpawnEffect = getIfExistsOpt<bool>(objectDynData->value, NewConstants::NOTE_SPAWN_EFFECT)
                                         ?: getIfExistsOpt<bool>(objectDynData->value, NewConstants::V2_DISABLE_SPAWN_EFFECT);
      if (v2 && chromaObjectData.SpawnEffect.has_value()) {
        chromaObjectData.SpawnEffect = !chromaObjectData.SpawnEffect.value();
      }
    } else if (ASSIGNMENT_CHECK(CustomSliderDataKlass, beatmapObjectData->klass)) {
      debugSpamLog("Custom note {}", il2cpp_utils::ClassStandardName(beatmapObjectData->klass).c_str());
      auto* customNoteData = reinterpret_cast<CustomJSONData::CustomSliderData*>(beatmapObjectData);

      objectDynData = customNoteData->customData;

      chromaObjectData.Color = ChromaUtilities::GetColorFromData(objectDynData->value, v2);
      chromaObjectData.SpawnEffect = getIfExistsOpt<bool>(objectDynData->value, NewConstants::NOTE_SPAWN_EFFECT)
                                         ?: getIfExistsOpt<bool>(objectDynData->value, NewConstants::V2_DISABLE_SPAWN_EFFECT);
      if (v2 && chromaObjectData.SpawnEffect.has_value()) {
        chromaObjectData.SpawnEffect = !chromaObjectData.SpawnEffect.value();
      }
    } else if (ASSIGNMENT_CHECK(CustomObstacleDataKlass, beatmapObjectData->klass)) {
      auto* customObstacleData = reinterpret_cast<CustomJSONData::CustomObstacleData*>(beatmapObjectData);

      objectDynData = customObstacleData->customData;
      chromaObjectData.Color = ChromaUtilities::GetColorFromData(objectDynData->value, v2);
    }
    //                else if (false && ASSIGNMENT_CHECK(CustomWaypointDataKlass,beatmapObjectData->klass)) {
    //                    debugSpamLog("Custom waypoint");
    //                    auto *customBeatmapEvent =
    //                    il2cpp_utils::cast<CustomJSONData::CustomWaypointData>(beatmapObjectData);
    //
    //                    // TODO: uncomment when CJD adds customData
    //                    // bool isCustomData = customBeatmapEvent->customData && customBeatmapEvent->customData->value
    //                    &&
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
    else {
      continue;
    }

    if (objectDynData->value) {
      rapidjson::Value const& customData = *objectDynData->value;
      auto const& animationObjectDyn =
          customData.FindMember(v2 ? Chroma::NewConstants::V2_ANIMATION.data() : Chroma::NewConstants::ANIMATION.data());
      if (animationObjectDyn != customData.MemberEnd()) {
        auto localColor =
            beatmapAD.getPointDefinition(animationObjectDyn->value, v2 ? Chroma::NewConstants::V2_COLOR : Chroma::NewConstants::COLOR,
                                         Tracks::ffi::WrapBaseValueType::Vec4);

        chromaObjectData.LocalPathColor = localColor != nullptr ? std::make_optional(localColor) : std::nullopt;
      }
    }
    auto const& tracks = TracksAD::getAD(objectDynData).tracks;
    chromaObjectData.Tracks = tracks;

    ChromaObjectDatas.try_emplace(beatmapObjectData, chromaObjectData);
  }
}
