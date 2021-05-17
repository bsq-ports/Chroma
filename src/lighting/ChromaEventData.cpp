#include "lighting/ChromaEventData.hpp"

#include "GlobalNamespace/BeatmapData.hpp"
#include "System/Collections/Generic/List_1.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"

#include "utils/ChromaUtils.hpp"

using namespace ChromaUtils;

std::optional<float> getIfExistsFloatOpt(std::optional<std::reference_wrapper<rapidjson::Value>> val, const std::string& member) {
    if (!val ||  val->get().MemberCount() == 0 || !val->get().IsObject()) return std::nullopt;

    auto it = val->get().FindMember(member);
    if (it == val->get().MemberEnd()) return std::nullopt;


    return it->value.GetFloat();
}

float getIfExistsFloat(std::optional<std::reference_wrapper<rapidjson::Value>> val, const std::string& member, float def) {
    if (!val || !val->get().IsObject() || val->get().Empty()) return def;

    auto it = val->get().FindMember(member);
    if (it == val->get().MemberEnd()) return def;
    return it->value.GetFloat();
}

void Chroma::ChromaEventDataManager::deserialize(GlobalNamespace::IReadonlyBeatmapData* beatmapData) {
    ChromaEventDatas.clear();

    static auto contextLogger = getLogger().WithContext(ChromaLogger::ObjectDataDeserialize);

    auto beatmapDataCast = il2cpp_utils::cast<GlobalNamespace::BeatmapData>(beatmapData);
    auto beatmapEvents = il2cpp_utils::cast<System::Collections::Generic::List_1<GlobalNamespace::BeatmapEventData*>>(beatmapDataCast->get_beatmapEventsData());

    for (int i = 0; i < beatmapEvents->items->Length(); i++) {
        auto beatmapEventData = beatmapEvents->items->get(i);


        auto customBeatmapEvent = il2cpp_utils::try_cast<CustomJSONData::CustomBeatmapEventData>(beatmapEventData);
        if (customBeatmapEvent) {
            std::shared_ptr<ChromaEventData> chromaEventData;



            std::optional<std::reference_wrapper<rapidjson::Value>> optionalDynData = (*customBeatmapEvent)->customData->value;

            switch ((int) (*customBeatmapEvent)->type) {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4: {
                    std::optional<ChromaLightEventData::GradientObjectData> gradientObject = std::nullopt;

                    debugSpamLog(contextLogger, "Light gradient");



                    if (optionalDynData) {
                        rapidjson::Value& unwrappedData = *optionalDynData;
                        auto gradientJSON = unwrappedData.FindMember(LIGHTGRADIENT);
                        if (gradientJSON != unwrappedData.MemberEnd()) {
                            auto &gValue = gradientJSON->value;

                            float duration = gValue.FindMember(Chroma::DURATION)->value.GetFloat(); // Trees.at(gradientObject, DURATION);

                            UnityEngine::Color initcolor = ChromaUtils::ChromaUtilities::GetColorFromData(gValue,
                                                                                                          STARTCOLOR).value();

                            UnityEngine::Color endcolor = ChromaUtils::ChromaUtilities::GetColorFromData(gValue,
                                                                                                         ENDCOLOR).value();

                            std::string easingString = std::string(gValue.FindMember(EASING)->value.GetString());

                            ChromaUtils::Functions easing;

                            if (easingString.empty()) {
                                easing = ChromaUtils::Functions::easeLinear;
                            } else {
                                auto s = ChromaUtils::FUNCTION_NAMES;
                                easing = (ChromaUtils::Functions) s[easingString];
                            }

                            gradientObject = std::make_optional(ChromaLightEventData::GradientObjectData{
                                    duration,
                                    initcolor,
                                    endcolor,
                                    easing
                            });
                        }
                    }

                    std::optional<std::reference_wrapper<rapidjson::Value>> lightIdOpt;
                    std::optional<std::reference_wrapper<rapidjson::Value>> propIdOpt;

                    // rapidjson really wants us to do empty checks why
                    if (optionalDynData) {
                        rapidjson::Value& unwrappedData = *optionalDynData;
                        debugSpamLog(contextLogger, "Light ID");
                        auto lightId = unwrappedData.FindMember(LIGHTID);
                        auto propId = unwrappedData.FindMember(PROPAGATIONID);
                        debugSpamLog(contextLogger, "Done ");

                        lightIdOpt = lightId == unwrappedData.MemberEnd() ? std::nullopt : std::make_optional(
                                std::ref(lightId->value));


                        propIdOpt = propId == unwrappedData.MemberEnd() ? std::nullopt : std::make_optional(
                                std::ref(propId->value));
                    }

                    auto chromaLightEvent = std::make_shared<ChromaLightEventData>();
                    chromaLightEvent->LightID = lightIdOpt;
                    chromaLightEvent->PropID = propIdOpt;
                    chromaLightEvent->ColorData = ChromaUtilities::GetColorFromData(optionalDynData);
                    chromaLightEvent->GradientObject = gradientObject;

                    chromaEventData = chromaLightEvent;
                    break;
                }
                case 8:
                    {
                    // https://github.com/Aeroluna/Chroma/commit/3900969d3fef1eaeea745bcfc23c61bfbe525586#diff-e83fa5da7e2e725f2cfb2ee5a6d6a085b2065a95e0d4757e01fe3c29f0fa4024

                    std::optional<std::string> NameFilter = getIfExists<std::string>(optionalDynData, NAMEFILTER);
                    std::optional<bool> reset = getIfExists<bool>(optionalDynData, RESET);
                    std::optional<bool> counterSpin = getIfExists<bool>(optionalDynData, COUNTERSPIN);


                    std::optional<bool> direction = getIfExists<bool>(optionalDynData, DIRECTION);
                    std::optional<float> step = getIfExistsFloatOpt(optionalDynData, STEP);
                    std::optional<float> prop = getIfExistsFloatOpt(optionalDynData, PROP);
                    std::optional<float> speed = getIfExistsFloatOpt(optionalDynData, SPEED);
                    std::optional<float> rotation = getIfExistsFloatOpt(optionalDynData, ROTATION);

                    auto stepMult = getIfExistsFloat(optionalDynData, STEPMULT, 1.0f);
                    auto propMult = getIfExistsFloat(optionalDynData, PROPMULT, 1.0f);
                    auto speedMult = getIfExistsFloat(optionalDynData, SPEEDMULT, 1.0f);

                    auto chromaRingRotationEventData = std::make_shared<ChromaRingRotationEventData>();

                    chromaRingRotationEventData->NameFilter = NameFilter;
                    chromaRingRotationEventData->Direction = direction;
                    chromaRingRotationEventData->CounterSpin = counterSpin;
                    chromaRingRotationEventData->Reset = reset;


                    chromaRingRotationEventData->Prop = prop;
                    chromaRingRotationEventData->Step = step;
                    chromaRingRotationEventData->Speed = speed;
                    chromaRingRotationEventData->Rotation = rotation;

                    chromaRingRotationEventData->StepMult = stepMult;
                    chromaRingRotationEventData->PropMult = propMult;
                    chromaRingRotationEventData->SpeedMult = speedMult;


                    chromaEventData = chromaRingRotationEventData;

                    break;
                }
                case 9: {
                    auto chromaRingRotationEventData = std::make_shared<ChromaRingStepEventData>();

                    chromaRingRotationEventData->Step = getIfExists<float>(optionalDynData, STEP);

                    chromaEventData = chromaRingRotationEventData;
                    break;
                }

                case 12:
                case 13: {
                    auto chromaLaserSpeedEventData = std::make_shared<ChromaLaserSpeedEventData>();

                    chromaLaserSpeedEventData->LockPosition = getIfExists<bool>(optionalDynData, LOCKPOSITION, false);
                    chromaLaserSpeedEventData->PreciseSpeed = getIfExistsFloat(optionalDynData, SPEED, getIfExistsFloat(optionalDynData, PRECISESPEED, (float) beatmapEventData->value));
                    chromaLaserSpeedEventData->Direction = getIfExists(optionalDynData, DIRECTION, -1);

                    chromaEventData = chromaLaserSpeedEventData;
                    break;
                }
                default:
                    continue;
            }
            ChromaEventDatas[beatmapEventData] = chromaEventData;
        }
    }
}
