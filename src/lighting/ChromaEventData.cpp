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

            debugSpamLog(contextLogger, "Light gradient");


            if (optionalDynData) {
                rapidjson::Value &unwrappedData = *optionalDynData;

                auto gradientJSON = unwrappedData.FindMember(LIGHTGRADIENT);
                if (gradientJSON != unwrappedData.MemberEnd() && !gradientJSON->value.IsNull() && gradientJSON->value.IsObject()) {
                    auto &gValue = gradientJSON->value;

                    float duration = gValue.FindMember(Chroma::DURATION)->value.GetFloat(); // Trees.at(gradientObject, DURATION);

                    Sombrero::FastColor initcolor = ChromaUtils::ChromaUtilities::GetColorFromData(gValue,STARTCOLOR).value();

                    Sombrero::FastColor endcolor = ChromaUtils::ChromaUtilities::GetColorFromData(gValue,ENDCOLOR).value();

                    std::string easingString = std::string(gValue.FindMember(EASING)->value.GetString());

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
            }

            std::optional<std::reference_wrapper<rapidjson::Value>> lightIdOpt;
            std::optional<std::reference_wrapper<rapidjson::Value>> propIdOpt;

            // rapidjson really wants us to do empty checks why
            if (optionalDynData) {
                rapidjson::Value &unwrappedData = *optionalDynData;
                debugSpamLog(contextLogger, "Light ID");
                auto lightId = unwrappedData.FindMember(LIGHTID);
                auto propId = unwrappedData.FindMember(PROPAGATIONID);
                debugSpamLog(contextLogger, "Done ");

                lightIdOpt = lightId == unwrappedData.MemberEnd() ? std::nullopt : std::make_optional(
                        std::ref(lightId->value));


                propIdOpt = propId == unwrappedData.MemberEnd() ? std::nullopt : std::make_optional(
                        std::ref(propId->value));
            }

            // RING STUFF
            std::optional<std::string> NameFilter = getIfExists<std::string>(optionalDynData, NAMEFILTER);
            std::optional<bool> reset = getIfExists<bool>(optionalDynData, RESET);
            std::optional<bool> counterSpin = getIfExists<bool>(optionalDynData, COUNTERSPIN);


            std::optional<int> direction = getIfExists<int>(optionalDynData, DIRECTION);
            std::optional<float> step = getIfExistsFloatOpt(optionalDynData, STEP);
            std::optional<float> prop = getIfExistsFloatOpt(optionalDynData, PROP);
            std::optional<float> speed = getIfExistsFloatOpt(optionalDynData, SPEED);

            if (!speed)
                speed = getIfExistsFloatOpt(optionalDynData, PRECISESPEED);

            std::optional<float> rotation = getIfExistsFloatOpt(optionalDynData, ROTATION);

            auto stepMult = getIfExistsFloat(optionalDynData, STEPMULT, 1.0f);
            auto propMult = getIfExistsFloat(optionalDynData, PROPMULT, 1.0f);
            auto speedMult = getIfExistsFloat(optionalDynData, SPEEDMULT, 1.0f);

            // ASSIGN
            std::shared_ptr<ChromaEventData> chromaEventData = std::make_shared<ChromaEventData>();
            chromaEventData->LightID = lightIdOpt;
            chromaEventData->PropID = propIdOpt;
            chromaEventData->ColorData = ChromaUtilities::GetColorFromData(optionalDynData);
            chromaEventData->GradientObject = gradientObject;

            chromaEventData->NameFilter = NameFilter;
            chromaEventData->Direction = direction;
            chromaEventData->CounterSpin = counterSpin;
            chromaEventData->Reset = reset;


            chromaEventData->Prop = prop;
            chromaEventData->Step = step;
            chromaEventData->Speed = speed;
            chromaEventData->Rotation = rotation;

            chromaEventData->StepMult = stepMult;
            chromaEventData->PropMult = propMult;
            chromaEventData->SpeedMult = speedMult;

            chromaEventData->LockPosition = getIfExists<bool>(optionalDynData, LOCKPOSITION, false);

            chromaEventData = chromaEventData;


            ChromaEventDatas[beatmapEventData] = chromaEventData;
        }
    }
}
