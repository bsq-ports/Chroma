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

    for (int i = 0; i < beatmapEvents->items->Length(); i++) {
        auto beatmapEventData = beatmapEvents->items->get(i);


        auto customBeatmapEvent = il2cpp_utils::try_cast<CustomJSONData::CustomBeatmapEventData>(beatmapEventData);
        if (customBeatmapEvent) {
            std::optional<std::reference_wrapper<rapidjson::Value>> optionalDynData = (*customBeatmapEvent)->customData->value;


            std::optional<ChromaEventData::GradientObjectData> gradientObject = std::nullopt;
            std::optional<std::reference_wrapper<const rapidjson::Value>> lightIdOpt;
            std::optional<std::reference_wrapper<const rapidjson::Value>> propIdOpt;

            debugSpamLog(contextLogger, "Light gradient");

            // ASSIGN
            auto chromaEventData = std::make_shared<ChromaEventData>();

            if (optionalDynData) {
                rapidjson::Value const &unwrappedData = *optionalDynData;

                auto gradientJSON = unwrappedData.FindMember(LIGHTGRADIENT);
                if (gradientJSON != unwrappedData.MemberEnd() && !gradientJSON->value.IsNull() && gradientJSON->value.IsObject()) {
                    auto const &gValue = gradientJSON->value;

                    float duration = gValue.FindMember(Chroma::DURATION)->value.GetFloat(); // Trees.at(gradientObject, DURATION);

                    Sombrero::FastColor initcolor = ChromaUtils::ChromaUtilities::GetColorFromData(gValue, STARTCOLOR).value();

                    Sombrero::FastColor endcolor = ChromaUtils::ChromaUtilities::GetColorFromData(gValue, ENDCOLOR).value();

                    auto easingString = std::string(gValue.FindMember(EASING)->value.GetString());

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

                lightIdOpt = lightId == unwrappedData.MemberEnd() ? std::nullopt : std::make_optional(
                        std::ref(lightId->value));


                propIdOpt = propId == unwrappedData.MemberEnd() ? std::nullopt : std::make_optional(
                        std::ref(propId->value));


                // Light stuff
                chromaEventData->LightID = lightIdOpt;
                chromaEventData->PropID = propIdOpt;
                chromaEventData->ColorData = ChromaUtilities::GetColorFromData(optionalDynData);
                chromaEventData->GradientObject = gradientObject;

                // RING STUFF
                chromaEventData->NameFilter = getIfExists<std::string>(optionalDynData, NAMEFILTER);
                chromaEventData->Direction = getIfExists<int>(optionalDynData, DIRECTION);
                chromaEventData->CounterSpin = getIfExists<bool>(optionalDynData, COUNTERSPIN);
                chromaEventData->Reset = getIfExists<bool>(optionalDynData, RESET);

                std::optional<float> speed = getIfExists<float>(optionalDynData, SPEED);

                if (!speed)
                    speed = getIfExists<float>(optionalDynData, PRECISESPEED);

                chromaEventData->Prop = getIfExists<float>(optionalDynData, PROP);
                chromaEventData->Step = getIfExists<float>(optionalDynData, STEP);
                chromaEventData->Speed = speed;
                chromaEventData->Rotation = getIfExists<float>(optionalDynData, ROTATION);
            }

            chromaEventData->StepMult = getIfExists<float>(optionalDynData, STEPMULT, 1.0f);
            chromaEventData->PropMult = getIfExists<float>(optionalDynData, PROPMULT, 1.0f);
            chromaEventData->SpeedMult = getIfExists<float>(optionalDynData, SPEEDMULT, 1.0f);


            // Light stuff again
            chromaEventData->LockPosition = getIfExists<bool>(optionalDynData, LOCKPOSITION, false);


            ChromaEventDatas[beatmapEventData] = chromaEventData;
        }
    }
}
