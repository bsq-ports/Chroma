#include "ChromaEventData.hpp"

#include "GlobalNamespace/BeatmapData.hpp"
#include "System/Collections/Generic/List_1.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"

#include "utils/ChromaUtils.hpp"

using namespace ChromaUtils;

void Chroma::ChromaEventDataManager::deserialize(GlobalNamespace::IReadonlyBeatmapData* beatmapData) {
    ChromaEventDatas.clear();

    auto beatmapDataCast = reinterpret_cast<GlobalNamespace::BeatmapData*>(beatmapData);
    auto beatmapEvents = reinterpret_cast<System::Collections::Generic::List_1<GlobalNamespace::BeatmapEventData*>*>(beatmapDataCast->get_beatmapEventsData());

    for (int i = 0; i < beatmapEvents->items->Length(); i++) {
        auto event = beatmapEvents->items->values[i];
        if (event && ASSIGNMENT_CHECK(classof(CustomJSONData::CustomBeatmapEventData*), event->klass)) {
            std::shared_ptr<ChromaEventData> chromaEventData;

            auto *customBeatmapEvent = reinterpret_cast<CustomJSONData::CustomBeatmapEventData *>(event);

            bool isCustomData = customBeatmapEvent->customData && customBeatmapEvent->customData->value && customBeatmapEvent->customData->value->IsObject();
            rapidjson::Value* dynData = isCustomData ? customBeatmapEvent->customData->value : nullptr;

            switch ((int) customBeatmapEvent->type) {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4: {
                    std::optional<ChromaLightEventData::GradientObjectData> gradientObject = std::nullopt;

                    auto gradientJSON = dynData->FindMember(LIGHTGRADIENT);
                    if (gradientJSON != dynData->MemberEnd()) {
                        float duration = gradientJSON->value.FindMember(
                                Chroma::DURATION)->value.GetFloat(); // Trees.at(gradientObject, DURATION);

                        UnityEngine::Color initcolor = ChromaUtils::ChromaUtilities::GetColorFromData(
                                gradientJSON->value,
                                STARTCOLOR).value();

                        UnityEngine::Color endcolor = ChromaUtils::ChromaUtilities::GetColorFromData(
                                gradientJSON->value,
                                ENDCOLOR).value();

                        std::string easingString = std::string(
                                gradientJSON->value.FindMember(EASING)->value.GetString());

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

                    auto lightId = dynData->FindMember(LIGHTID);
                    auto propId = dynData->FindMember(PROPAGATIONID);

                    auto chromaLightEvent = new ChromaLightEventData();
                    chromaLightEvent->LightID = lightId == dynData->MemberEnd() ? std::nullopt : std::make_optional(
                            &lightId->value);
                    chromaLightEvent->PropID = propId == dynData->MemberEnd() ? std::nullopt : std::make_optional(
                            &propId->value);
                    chromaLightEvent->ColorData = ChromaUtilities::GetColorFromData(dynData);
                    chromaLightEvent->GradientObject = gradientObject;

                    chromaEventData = std::shared_ptr<ChromaEventData>(chromaLightEvent);
                    break;
                }
                case 8:
                case 9: {
                    auto nameFilter = dynData->FindMember(NAMEFILTER);

                    // https://github.com/Aeroluna/Chroma/commit/3900969d3fef1eaeea745bcfc23c61bfbe525586#diff-e83fa5da7e2e725f2cfb2ee5a6d6a085b2065a95e0d4757e01fe3c29f0fa4024



                    std::optional<std::string> NameFilter = getIfExists<std::string>(dynData, NAMEFILTER);
                    std::optional<bool> reset = getIfExists<bool>(dynData, RESET);
                    std::optional<bool> counterSpin = getIfExists<bool>(dynData, COUNTERSPIN);


                    std::optional<bool> direction = getIfExists<bool>(dynData, DIRECTION);
                    std::optional<float> step = getIfExists<float>(dynData, STEP);
                    std::optional<float> prop = getIfExists<float>(dynData, PROP);
                    std::optional<float> speed = getIfExists<float>(dynData, SPEED);
                    std::optional<float> rotation = getIfExists<float>(dynData, ROTATION);

                    auto stepMult = getIfExists<float>(dynData, STEPMULT, 1.0f);
                    auto propMult = getIfExists<float>(dynData, PROPMULT, 1.0f);
                    auto speedMult = getIfExists<float>(dynData, SPEEDMULT, 1.0f);

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


                    chromaEventData = std::shared_ptr<ChromaEventData>(chromaRingRotationEventData);

                    break;
                }
                case 12:
                case 13: {
                    auto chromaLaserSpeedEventData = new ChromaLaserSpeedEventData();

                    chromaLaserSpeedEventData->LockPosition = getIfExists(dynData, LOCKPOSITION, false);
                    chromaLaserSpeedEventData->PreciseSpeed = getIfExists(dynData, PRECISESPEED,
                                                                          customBeatmapEvent->value);
                    chromaLaserSpeedEventData->Direction = getIfExists(dynData, DIRECTION, -1);

                    chromaEventData = std::shared_ptr<ChromaEventData>(chromaLaserSpeedEventData);
                    break;
                }
                default:
                    continue;
            }
            ChromaEventDatas[event] = chromaEventData;
        }
    }
}
