#include "lighting/ChromaEventData.hpp"

#include "GlobalNamespace/BeatmapData.hpp"
#include "System/Collections/Generic/List_1.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"

#include "utils/ChromaUtils.hpp"

using namespace ChromaUtils;

std::optional<float> getIfExistsFloatOpt(rapidjson::Value*& val, const std::string& member) {
    if (!val ||  val->MemberCount() == 0 || !val->IsObject()) return std::nullopt;

    auto it = val->FindMember(member);
    if (it == val->MemberEnd()) return std::nullopt;


    return it->value.GetFloat();
}

float getIfExistsFloat(rapidjson::Value* val, const std::string& member, float def) {
    if (!val || !val->IsObject() || val->Empty()) return def;

    auto it = val->FindMember(member);
    if (it == val->MemberEnd()) return def;
    return it->value.GetFloat();
}

void Chroma::ChromaEventDataManager::deserialize(GlobalNamespace::IReadonlyBeatmapData* beatmapData) {
    ChromaEventDatas.clear();

    static auto contextLogger = getLogger().WithContext(ChromaLogger::ObjectDataDeserialize);

    auto beatmapDataCast = reinterpret_cast<GlobalNamespace::BeatmapData*>(beatmapData);
    auto beatmapEvents = reinterpret_cast<System::Collections::Generic::List_1<GlobalNamespace::BeatmapEventData*>*>(beatmapDataCast->get_beatmapEventsData());

    for (int i = 0; i < beatmapEvents->items->Length(); i++) {
        auto beatmapEventData = beatmapEvents->items->values[i];
        if (beatmapEventData && ASSIGNMENT_CHECK(classof(CustomJSONData::CustomBeatmapEventData *), beatmapEventData->klass)) {
            std::shared_ptr<ChromaEventData> chromaEventData;

            auto *customBeatmapEvent = reinterpret_cast<CustomJSONData::CustomBeatmapEventData *>(beatmapEventData);

            rapidjson::Value* dynData = customBeatmapEvent->customData->value;

            switch ((int) customBeatmapEvent->type) {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4: {
                    std::optional<ChromaLightEventData::GradientObjectData> gradientObject = std::nullopt;

                    debugSpamLog(contextLogger, "Light gradient");

                    auto empty = !dynData || dynData->MemberCount() == 0;

                    if (!empty) {
                        auto gradientJSON = dynData->FindMember(LIGHTGRADIENT);
                        if (gradientJSON != dynData->MemberEnd()) {
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

                    std::optional<rapidjson::Value *> lightIdOpt;
                    std::optional<rapidjson::Value *> propIdOpt;

                    // rapidjson really wants us to do empty checks why
                    if (!empty) {
                        debugSpamLog(contextLogger, "Light ID");
                        auto lightId = dynData->FindMember(LIGHTID);
                        auto propId = dynData->FindMember(PROPAGATIONID);
                        debugSpamLog(contextLogger, "Done ");

                        lightIdOpt = lightId == dynData->MemberEnd() ? std::nullopt : std::make_optional(
                                &lightId->value);


                        propIdOpt = propId == dynData->MemberEnd() ? std::nullopt : std::make_optional(
                                &propId->value);
                    }

                    auto chromaLightEvent = new ChromaLightEventData();
                    chromaLightEvent->LightID = lightIdOpt;
                    chromaLightEvent->PropID = propIdOpt;
                    chromaLightEvent->ColorData = ChromaUtilities::GetColorFromData(dynData);
                    chromaLightEvent->GradientObject = gradientObject;

                    chromaEventData = std::shared_ptr<ChromaLightEventData>(chromaLightEvent);
                    break;
                }
                case 8:
                case 9: {
                    // https://github.com/Aeroluna/Chroma/commit/3900969d3fef1eaeea745bcfc23c61bfbe525586#diff-e83fa5da7e2e725f2cfb2ee5a6d6a085b2065a95e0d4757e01fe3c29f0fa4024

                    std::optional<std::string> NameFilter = getIfExists<std::string>(dynData, NAMEFILTER);
                    std::optional<bool> reset = getIfExists<bool>(dynData, RESET);
                    std::optional<bool> counterSpin = getIfExists<bool>(dynData, COUNTERSPIN);


                    std::optional<bool> direction = getIfExists<bool>(dynData, DIRECTION);
                    std::optional<float> step = getIfExistsFloatOpt(dynData, STEP);
                    std::optional<float> prop = getIfExistsFloatOpt(dynData, PROP);
                    std::optional<float> speed = getIfExistsFloatOpt(dynData, SPEED);
                    std::optional<float> rotation = getIfExistsFloatOpt(dynData, ROTATION);

                    auto stepMult = getIfExistsFloat(dynData, STEPMULT, 1.0f);
                    auto propMult = getIfExistsFloat(dynData, PROPMULT, 1.0f);
                    auto speedMult = getIfExistsFloat(dynData, SPEEDMULT, 1.0f);

                    auto chromaRingRotationEventData = new ChromaRingRotationEventData();

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


                    chromaEventData = std::shared_ptr<ChromaRingRotationEventData>(chromaRingRotationEventData);

                    break;
                }
                case 12:
                case 13: {
                    auto chromaLaserSpeedEventData = new ChromaLaserSpeedEventData();

                    chromaLaserSpeedEventData->LockPosition = getIfExists<bool>(dynData, LOCKPOSITION, false);
                    chromaLaserSpeedEventData->PreciseSpeed = getIfExistsFloat(dynData, PRECISESPEED, (float) beatmapEventData->value);
                    chromaLaserSpeedEventData->Direction = getIfExists(dynData, DIRECTION, -1);

                    chromaEventData = std::shared_ptr<ChromaLaserSpeedEventData>(chromaLaserSpeedEventData);
                    break;
                }
                default:
                    continue;
            }
            ChromaEventDatas[beatmapEventData] = chromaEventData;
        }
    }
}
