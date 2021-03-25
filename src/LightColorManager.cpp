#include "Chroma.hpp"
#include "LightColorManager.hpp"
#include "colorizer/LightColorizer.hpp"
#include "LegacyLightHelper.hpp"
#include "utils/ChromaUtils.hpp"
#include "ChromaGradientController.hpp"
#include "hooks/LightSwitchEventEffect.hpp"


using namespace Chroma;
using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;

void Chroma::LightColorManager::ColorLightSwitch(MonoBehaviour* monobehaviour, BeatmapEventData* beatmapEventData) {
    static auto contextLogger = getLogger().WithContext(ChromaLogger::ColorLightSwitch);

    LightColorizer::SetLastValue(monobehaviour, beatmapEventData->value);

    std::optional<UnityEngine::Color> color = LegacyLightHelper::GetLegacyColor(beatmapEventData);

    debugSpamLog(contextLogger, "Color is legacy? %s", color ? "true" : "false");



    if (il2cpp_functions::class_is_assignable_from(classof(CustomBeatmapEventData*), beatmapEventData->klass)) {
        auto* customBeatmapEvent = reinterpret_cast<CustomBeatmapEventData *>(beatmapEventData);
        // TODO: WHY DOES THIS CAUSE A CRASH? WHY
        // signal 11 (SIGSEGV), code 1 (SEGV_MAPERR), fault addr 0x650084
        auto *customDataWrapper = customBeatmapEvent->customData;

        debugSpamLog(contextLogger, "Wrapper klass %s", il2cpp_utils::ClassStandardName(beatmapEventData->klass).c_str());

        if (customDataWrapper && customDataWrapper->value) {
            debugSpamLog(contextLogger, "JSON data 1");


            rapidjson::Value *dynData = customDataWrapper->value;
            if (il2cpp_functions::class_is_assignable_from(monobehaviour->klass, classof(LightSwitchEventEffect *))) {
                auto *lightSwitchEventEffect = reinterpret_cast<LightSwitchEventEffect *>(monobehaviour);

                // TODO: FIX OR REMOVE! This helps avoid a crash with CJD's rapidjson allocation.
                if (dynData->IsObject() || dynData->IsArray()) {

                    auto lightMember = dynData->FindMember("_lightID");
                    if (lightMember != dynData->MemberEnd()) {
                        debugSpamLog(contextLogger, "JSON data 2");
                        rapidjson::Value &lightIdData = lightMember->value;
                        std::vector<ILightWithId *> lights = LightColorizer::GetLights(lightSwitchEventEffect);
                        int lightCount = lights.size();

                        if (lightIdData.IsInt() || lightIdData.IsInt64()) {
                            auto lightIdLong = lightIdData.GetInt();
                            if (lightCount > lightIdLong) {
                                std::vector<ILightWithId *> overrideLights;

                                overrideLights.push_back(lights[lightIdLong]);

                                LightSwitchEventEffectHolder::OverrideLightWithIdActivation = std::make_optional(
                                        overrideLights);
                            }
                        } else {
                            // It's a list
                            auto lightIDobjects = lightIdData.GetObject();
                            std::vector<int> lightIDArray;


                            for (auto &lightId : lightIDobjects) {
                                lightIDArray.push_back(lightId.value.GetInt());
                            }

                            std::vector<ILightWithId *> overrideLights;

                            for (auto lightId : lightIDArray) {
                                if (lightCount > lightId) {
                                    overrideLights.push_back(lights[lightId]);
                                }
                            }

                            LightSwitchEventEffectHolder::OverrideLightWithIdActivation = std::make_optional(
                                    overrideLights);
                        }
                    }

                    auto propMember = dynData->FindMember("_propID");
                    if (propMember != dynData->MemberEnd()) {
                        debugSpamLog(contextLogger, "JSON data 3");
                        rapidjson::Value &propIDData = propMember->value;

                        std::unordered_map<int, std::vector<ILightWithId *>> lights = LightColorizer::GetLightsPropagationGrouped(
                                lightSwitchEventEffect);
                        int lightCount = lights.size();

                        debugSpamLog(contextLogger, "Prop id data is");
                        PrintJSONValue(propIDData);

                        if (propIDData.IsInt64() || propIDData.IsInt()) {
                            auto propIdLong = propIDData.GetInt();
                            debugSpamLog(contextLogger, "It is an int prop %d %d", lightCount, propIdLong);
                            if (lightCount > propIdLong) {
                                LightSwitchEventEffectHolder::OverrideLightWithIdActivation = std::make_optional(
                                        lights[propIdLong]);
                            }
                        } else {
                            debugSpamLog(contextLogger, "It is a list prop");
                            // It's a list
                            auto propIDobjects = propIDData.GetObject();
                            std::vector<int> propIDArray;


                            for (auto &lightId : propIDobjects) {
                                propIDArray.push_back(lightId.value.GetInt());
                            }

                            std::vector<ILightWithId *> overrideLights;

                            for (auto propId : propIDArray) {
                                if (lightCount > propId) {
                                    for (auto l : lights[propId]) {
                                        overrideLights.push_back(l);
                                    }
                                }
                            }

                            LightSwitchEventEffectHolder::OverrideLightWithIdActivation = std::make_optional(overrideLights);
                        }
                    }

                    //gradientObject
                    // FIXME, THIS IS VERY TEMP!!
                    auto gradient = dynData->FindMember("_lightGradient");
                    if (gradient != dynData->MemberEnd()) {
                        color = ChromaGradientController::AddGradient(&gradient->value, beatmapEventData->type, beatmapEventData->time);
                    }
                }
            }

            std::optional<UnityEngine::Color> colorData = dynData
                    ? ChromaUtils::ChromaUtilities::GetColorFromData(dynData)
                    : std::nullopt;
            if (colorData) {
                color = colorData;
                ChromaGradientController::CancelGradient(beatmapEventData->type);
            }
        }
    }


    if (color) {
        LightColorizer::SetLightingColors(monobehaviour, color, color, color, color);
    } else if (!ChromaGradientController::IsGradientActive(beatmapEventData->type)) {
        LightColorizer::Reset(monobehaviour);
    }
}