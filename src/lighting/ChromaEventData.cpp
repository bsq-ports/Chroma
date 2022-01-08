#include "lighting/ChromaEventData.hpp"

#include "GlobalNamespace/BeatmapData.hpp"
#include "System/Collections/Generic/List_1.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"

#include "utils/ChromaUtils.hpp"

using namespace ChromaUtils;


void Chroma::ChromaEventDataManager::deserialize(GlobalNamespace::IReadonlyBeatmapData* beatmapData) {
    static auto contextLogger = getLogger().WithContext(ChromaLogger::ObjectDataDeserialize);
    ChromaEventDatas.clear();

    auto beatmapDataCast = il2cpp_utils::cast<GlobalNamespace::BeatmapData>(beatmapData);
    auto beatmapEvents = il2cpp_utils::cast<System::Collections::Generic::List_1<GlobalNamespace::BeatmapEventData *>>(
            beatmapDataCast->get_beatmapEventsData());

    auto beatmapEventsLength = beatmapEvents->items->Length();
    for (int i = 0; i < beatmapEventsLength; i++) {
        auto beatmapEventData = beatmapEvents->items->get(i);

        if (!beatmapEventData)
            continue;

        auto customBeatmapEvent = il2cpp_utils::try_cast<CustomJSONData::CustomBeatmapEventData>(beatmapEventData);
        if (customBeatmapEvent) {
            std::optional<std::reference_wrapper<rapidjson::Value>> optionalDynData = (*customBeatmapEvent)->customData->value;


            std::optional<ChromaEventData::GradientObjectData> gradientObject = std::nullopt;

            debugSpamLog(contextLogger, "Light gradient");

            // ASSIGN
            ChromaEventData chromaEventData;

            if (optionalDynData) {
                rapidjson::Value const &unwrappedData = *optionalDynData;

                auto gradientJSON = unwrappedData.FindMember(LIGHTGRADIENT);
                if (gradientJSON != unwrappedData.MemberEnd() && !gradientJSON->value.IsNull() && gradientJSON->value.IsObject()) {
                    auto const &gValue = gradientJSON->value;

                    float duration = gValue.FindMember(Chroma::DURATION)->value.GetFloat(); // Trees.at(gradientObject, DURATION);

                    Sombrero::FastColor initcolor = ChromaUtils::ChromaUtilities::GetColorFromData(gValue, STARTCOLOR).value();

                    Sombrero::FastColor endcolor = ChromaUtils::ChromaUtilities::GetColorFromData(gValue, ENDCOLOR).value();

                    std::string_view easingString = gValue.FindMember(EASING)->value.GetString();

                    Functions easing;

                    if (easingString.empty()) {
                        easing = Functions::easeLinear;
                    } else {
                        easing = FunctionFromStr(easingString);
                    }

                    gradientObject = std::make_optional(ChromaEventData::GradientObjectData{
                            duration,
                            initcolor,
                            endcolor,
                            easing
                    });
                }

                debugSpamLog(contextLogger, "Light ID");
                auto lightId = unwrappedData.FindMember(LIGHTID);
                auto propId = unwrappedData.FindMember(PROPAGATIONID);
                debugSpamLog(contextLogger, "Done ");

                if (lightId != unwrappedData.MemberEnd()) {
                    rapidjson::Value const &lightIdData = lightId->value;
                    std::vector<int> lightIds;

                    if (lightIdData.IsNumber()) {
                        auto lightIdLong = lightIdData.GetInt64();
                        lightIds.push_back((int) lightIdLong);

                    } else if (lightIdData.IsArray()) {
                        lightIds.reserve(lightIdData.Size());
                        // It's an array
                        auto const &lightIDobjects = lightIdData.GetArray();
                        for (auto const& lightId: lightIDobjects) {
                            lightIds.push_back(lightId.GetInt());
                        }
                    } else {
                        getLogger().error("Light id type is not array or number!");
                    }

                    chromaEventData.LightID = lightIds;
                }


                // Prop ID is deprecated apparently.  https://github.com/Aeroluna/Chroma/commit/711cb19f7d03a1776a24cef52fd8ef6fd7685a2b#diff-b8fcfff3ebc4ceb7b43d8401d9f50750dc88326d0a87897c5593923e55b23879R41
                if (propId != unwrappedData.MemberEnd()) {
                    rapidjson::Value const &propIDData = propId->value;

                    std::vector<int> propIds;


                    if (propIDData.IsNumber()) {
                        auto propIdLong = propIDData.GetInt();
                        propIds.push_back(propIdLong);
                    } else {
                        // It's a list
                        if (propIDData.IsObject()) {
                            auto const &propIDobjects = propIDData.GetObject();
                            propIds.reserve(propIDobjects.MemberCount());

                            for (auto const &lightId: propIDobjects) {
                                auto propId = lightId.value.GetInt64();
                                propIds.push_back((int) propId);
                            }
                        } else if (propIDData.IsArray()) {
                            auto const &propIDArray = propIDData.GetArray();
                            propIds.reserve(propIDArray.Size());

                            for (auto const &lightId: propIDArray) {
                                auto propId = lightId.GetInt64();
                                propIds.push_back((int) propId);
                            }
                        } else {
                            getLogger().error("Prop id type is not array or number!");
                        }
                    }

                    chromaEventData.PropID = propIds;
                }

                // Light stuff
                chromaEventData.ColorData = ChromaUtilities::GetColorFromData(optionalDynData);
                chromaEventData.GradientObject = gradientObject;

                // RING STUFF
                chromaEventData.NameFilter = getIfExists<std::string>(optionalDynData, NAMEFILTER);
                chromaEventData.Direction = getIfExists<int>(optionalDynData, DIRECTION);
                chromaEventData.CounterSpin = getIfExists<bool>(optionalDynData, COUNTERSPIN);
                chromaEventData.Reset = getIfExists<bool>(optionalDynData, RESET);

                std::optional<float> speed = getIfExists<float>(optionalDynData, SPEED);

                if (!speed)
                    speed = getIfExists<float>(optionalDynData, PRECISESPEED);

                chromaEventData.Prop = getIfExists<float>(optionalDynData, PROP);
                chromaEventData.Step = getIfExists<float>(optionalDynData, STEP);
                chromaEventData.Speed = speed;
                chromaEventData.Rotation = getIfExists<float>(optionalDynData, ROTATION);
            }

            chromaEventData.StepMult = getIfExists<float>(optionalDynData, STEPMULT, 1.0f);
            chromaEventData.PropMult = getIfExists<float>(optionalDynData, PROPMULT, 1.0f);
            chromaEventData.SpeedMult = getIfExists<float>(optionalDynData, SPEEDMULT, 1.0f);


            // Light stuff again
            chromaEventData.LockPosition = getIfExists<bool>(optionalDynData, LOCKPOSITION, false);


            ChromaEventDatas.try_emplace(beatmapEventData, std::move(chromaEventData));
        }
    }
}
