#include "lighting/ChromaEventData.hpp"

#include "GlobalNamespace/BeatmapData.hpp"
#include "System/Collections/Generic/List_1.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"

#include "utils/ChromaUtils.hpp"

using namespace ChromaUtils;

void Chroma::ChromaEventDataManager::deserialize(CustomJSONData::CustomBeatmapData* beatmapData) {
  static auto contextLogger = getLogger().WithContext(ChromaLogger::ObjectDataDeserialize);
  ChromaEventDatas.clear();

  auto* beatmapDataCast = beatmapData;
  static auto* CustomBasicBeatmapEventDataKlass = classof(CustomJSONData::CustomBeatmapEventData*);

  bool v2 = beatmapDataCast->v2orEarlier;

  for (auto* beatmapEvent : beatmapDataCast->beatmapEventDatas) {

    if (beatmapEvent->klass != CustomBasicBeatmapEventDataKlass) {
      continue;
    }
    auto* customBeatmapEvent = reinterpret_cast<CustomJSONData::CustomBeatmapEventData*>(beatmapEvent);

    auto const& optionalDynData = customBeatmapEvent->customData->value;

    std::optional<ChromaEventData::GradientObjectData> gradientObject = std::nullopt;

    debugSpamLog(contextLogger, "Light gradient");

    // ASSIGN
    ChromaEventData chromaEventData;

    if (optionalDynData) {
      rapidjson::Value const& unwrappedData = *optionalDynData;

#pragma region V2 Gradients and prop
      if (v2) {
        auto gradientJSON = unwrappedData.FindMember(NewConstants::V2_LIGHT_GRADIENT.data());
        if (gradientJSON != unwrappedData.MemberEnd() && !gradientJSON->value.IsNull() &&
            gradientJSON->value.IsObject()) {
          auto const& gValue = gradientJSON->value;

          float duration = ChromaUtils::getIfExists<float>(gValue, Chroma::NewConstants::V2_DURATION).value_or(0);

          Sombrero::FastColor initcolor =
              ChromaUtils::ChromaUtilities::GetColorFromData(gValue, Chroma::NewConstants::V2_START_COLOR).value();

          Sombrero::FastColor endcolor =
              ChromaUtils::ChromaUtilities::GetColorFromData(gValue, Chroma::NewConstants::V2_END_COLOR).value();

          std::string_view easingString = gValue.FindMember(Chroma::NewConstants::V2_EASING.data())->value.GetString();

          Functions easing;

          if (easingString.empty()) {
            easing = Functions::easeLinear;
          } else {
            easing = FunctionFromStr(easingString);
          }

          gradientObject =
              std::make_optional(ChromaEventData::GradientObjectData{ duration, initcolor, endcolor, easing });
        }

        auto propId = unwrappedData.FindMember(Chroma::NewConstants::V2_PROPAGATION_ID.data());
        // Prop ID is deprecated apparently.
        // https://github.com/Aeroluna/Chroma/commit/711cb19f7d03a1776a24cef52fd8ef6fd7685a2b#diff-b8fcfff3ebc4ceb7b43d8401d9f50750dc88326d0a87897c5593923e55b23879R41
        if (propId != unwrappedData.MemberEnd()) {
          rapidjson::Value const& propIDData = propId->value;

          std::vector<int> propIds;

          if (propIDData.IsNumber()) {
            auto propIdLong = propIDData.GetInt();
            propIds.push_back(propIdLong);
          } else {
            // It's a list
            if (propIDData.IsObject()) {
              auto const& propIDobjects = propIDData.GetObject();
              propIds.reserve(propIDobjects.MemberCount());

              for (auto const& lightId : propIDobjects) {
                auto propId = lightId.value.GetInt64();
                propIds.push_back(static_cast<int>(propId));
              }
            } else if (propIDData.IsArray()) {
              auto const& propIDArray = propIDData.GetArray();
              propIds.reserve(propIDArray.Size());

              for (auto const& lightId : propIDArray) {
                auto propId = lightId.GetInt64();
                propIds.push_back(static_cast<int>(propId));
              }
            } else {
              getLogger().error("Prop id type is not array or number!");
            }
          }

          chromaEventData.PropID = propIds;
        }
      }
#pragma endregion
      auto easingString =
          getIfExistsOpt<std::string>(optionalDynData, v2 ? NewConstants::V2_EASING : NewConstants::EASING);

      if (easingString) {
        Functions easing;

        if (easingString->empty()) {
          easing = Functions::easeLinear;
        } else {
          easing = FunctionFromStr(*easingString);
        }

        chromaEventData.Easing = easing;
      }

      auto lerpTypeStr =
          getIfExistsOpt<std::string>(optionalDynData, v2 ? NewConstants::V2_LERP_TYPE : NewConstants::LERP_TYPE);

      if (lerpTypeStr) {
        LerpType lerpType;

        if (easingString->empty()) {
          lerpType = LerpType::RGB;
        } else {
          lerpType = LerpTypeFromString(*easingString);
        }

        chromaEventData.LerpType = lerpType;
      }

      debugSpamLog(contextLogger, "Light ID");
      auto lightId = unwrappedData.FindMember(v2 ? NewConstants::V2_LIGHT_ID.data() : NewConstants::LIGHT_ID.data());

      if (lightId != unwrappedData.MemberEnd()) {
        rapidjson::Value const& lightIdData = lightId->value;
        std::vector<int> lightIds;

        if (lightIdData.IsNumber()) {
          auto lightIdLong = lightIdData.GetInt64();
          lightIds.push_back(static_cast<int>(lightIdLong));

        } else if (lightIdData.IsArray()) {
          lightIds.reserve(lightIdData.Size());
          // It's an array
          auto const& lightIDobjects = lightIdData.GetArray();
          for (auto const& lightId : lightIDobjects) {
            lightIds.push_back(lightId.GetInt());
          }
        } else {
          getLogger().error("Light id type is not array or number!");
        }

        chromaEventData.LightID = lightIds;
      }

      // Light stuff
      chromaEventData.ColorData = ChromaUtilities::GetColorFromData(optionalDynData, v2);
      chromaEventData.GradientObject = gradientObject;

      // RING STUFF
      chromaEventData.NameFilter =
          getIfExistsOpt<std::string>(optionalDynData, v2 ? NewConstants::V2_NAME_FILTER : NewConstants::NAME_FILTER);
      chromaEventData.Direction =
          getIfExistsOpt<int>(optionalDynData, v2 ? NewConstants::V2_DIRECTION : NewConstants::DIRECTION);
      chromaEventData.CounterSpin =
          v2 ? getIfExistsOpt<bool>(optionalDynData, NewConstants::V2_COUNTER_SPIN) : std::nullopt;
      chromaEventData.Reset = v2 ? getIfExistsOpt<bool>(optionalDynData, NewConstants::V2_RESET) : std::nullopt;

      std::optional<float> speed =
          getIfExistsOpt<float>(optionalDynData, v2 ? NewConstants::V2_SPEED : NewConstants::SPEED);

      if (!speed && v2) {
        speed = getIfExistsOpt<float>(optionalDynData, NewConstants::V2_PRECISE_SPEED);
      }

      chromaEventData.Prop = getIfExistsOpt<float>(optionalDynData, v2 ? NewConstants::V2_PROP : NewConstants::PROP);
      chromaEventData.Step = getIfExistsOpt<float>(optionalDynData, v2 ? NewConstants::V2_STEP : NewConstants::STEP);
      chromaEventData.Speed = speed;
      chromaEventData.Rotation =
          getIfExistsOpt<float>(optionalDynData, v2 ? NewConstants::V2_ROTATION : NewConstants::ROTATION);
    }

    chromaEventData.StepMult = v2 ? getIfExistsOpt<float>(optionalDynData, NewConstants::V2_STEP_MULT, 1.0F) : 1;
    chromaEventData.PropMult = v2 ? getIfExistsOpt<float>(optionalDynData, NewConstants::V2_PROP_MULT, 1.0F) : 1;
    chromaEventData.SpeedMult = v2 ? getIfExistsOpt<float>(optionalDynData, NewConstants::V2_SPEED_MULT, 1.0F) : 1;

    // Light stuff again
    chromaEventData.LockPosition =
        getIfExistsOpt<bool>(optionalDynData, v2 ? NewConstants::V2_LOCK_POSITION : NewConstants::LOCK_POSITION, false);

    ChromaEventDatas.try_emplace(customBeatmapEvent, std::move(chromaEventData));
  }

  std::vector<int> allUsedIds;
  allUsedIds.reserve(beatmapDataCast->beatmapEventDatas.size());

  for (auto const& event : beatmapDataCast->beatmapEventDatas) {
    auto const& dataIt = ChromaEventDatas.find(event);
    if (dataIt == ChromaEventDatas.end()) {
      continue;
    }

    auto const& d = dataIt->second;

    if (!d.LightID) {
      continue;
    }

    for (auto const& l : *d.LightID) {
      if (std::find(allUsedIds.begin(), allUsedIds.end(), l) == allUsedIds.end()) {
        continue;
      }
      allUsedIds.push_back(l);
    }
  }

  // start at 0
  int i = -1;
  auto beatmapEventsLength = beatmapDataCast->beatmapEventDatas.size();

  for (auto* beatmapEventData : beatmapDataCast->beatmapEventDatas) {
    i++;

    auto tryCast = il2cpp_utils::try_cast<GlobalNamespace::BasicBeatmapEventData>(beatmapEventData);
    if (!tryCast) {
      continue;
    }

    auto* basicBeatmapEventData = *tryCast;
    auto chromaEventDataIt = ChromaEventDatas.find(basicBeatmapEventData);

    if (chromaEventDataIt == ChromaEventDatas.end()) {
      continue;
    }

    auto* customBeatmapEvent = static_cast<CustomJSONData::CustomBeatmapEventData*>(basicBeatmapEventData);
    auto& currentEventData = chromaEventDataIt->second;
    // Horrible stupid logic to get next same type event per light id

    auto type = customBeatmapEvent->basicBeatmapEventType;
    auto& nextSameTypeEvent = currentEventData.NextSameTypeEvent;

    auto const& ids = currentEventData.LightID.value_or(allUsedIds);

    for (int id : ids) {
      if (i >= beatmapEventsLength - 1) {
        continue;
      }

      int nextIndex = FindIndex(
          beatmapDataCast->beatmapEventDatas,
          [type, id](GlobalNamespace::BeatmapEventData* n) {
            auto tryCast = il2cpp_utils::try_cast<GlobalNamespace::BasicBeatmapEventData>(n);
            if (!tryCast) {
              return false;
            }

            if (tryCast.value()->basicBeatmapEventType != type) {
              return false;
            }

            auto it = ChromaEventDatas.find(n);

            if (it == ChromaEventDatas.end()) {
              return false;
            }

            ChromaEventData const& nextEventData = it->second;
            auto const& lightId = nextEventData.LightID;

            return !lightId || std::find(lightId->begin(), lightId->end(), id) != lightId->end();
          },
          i + 1);

      if (nextIndex != -1) {
        auto* beatmapEvent = beatmapDataCast->beatmapEventDatas[nextIndex];
        currentEventData.NextSameTypeEvent[id] = { beatmapEvent, &ChromaEventDatas.at(beatmapEvent) };
      }
    }
  }
}
