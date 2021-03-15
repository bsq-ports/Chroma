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

void Chroma::LightColorManager::ColorLightSwitch(MonoBehaviour* monobehaviour, CustomBeatmapEventData* beatmapEventData) {
    LightColorizer::SetLastValue(monobehaviour, beatmapEventData->value);

    std::optional<UnityEngine::Color> color = LegacyLightHelper::GetLegacyColor(beatmapEventData);

    // TODO: WHY DOES THIS CAUSE A CRASH? WHY
    // signal 11 (SIGSEGV), code 1 (SEGV_MAPERR), fault addr 0x650084
    auto* customDataWrapper = beatmapEventData->customData;

    if(customDataWrapper && customDataWrapper->value) {
        getLogger().debug("JSON data 1");


        rapidjson::Value *dynData = beatmapEventData->customData->value;
        if (il2cpp_functions::class_is_assignable_from(monobehaviour->klass, classof(LightSwitchEventEffect *))) {
            auto *lightSwitchEventEffect = reinterpret_cast<LightSwitchEventEffect *>(monobehaviour);

            // TODO: FIX OR REMOVE! This helps avoid a crash with CJD's rapidjson allocation.
            if (!dynData->Empty()) {

                if (dynData->HasMember("_lightID")) {
                    getLogger().debug("JSON data 2");
                    rapidjson::Value &lightIdData = dynData->FindMember("_lightID")->value;
                    std::vector<ILightWithId *> lights = LightColorizer::GetLights(lightSwitchEventEffect);
                    int lightCount = lights.size();

                    if (lightIdData.IsInt() || lightIdData.IsInt64()) {
                        auto lightIdLong = lightIdData.GetInt();
                        if (lightCount > lightIdLong) {
                            std::vector<ILightWithId *> overrideLights;

                            overrideLights.push_back(lights[lightIdLong]);

                            OverrideLightWithIdActivation = std::make_optional(overrideLights);
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

                        OverrideLightWithIdActivation = std::make_optional(overrideLights);
                    }
                }

                if (dynData->HasMember("_propID")) {
                    getLogger().debug("JSON data 3");
                    rapidjson::Value &propIDData = dynData->FindMember("_propID")->value;

                    std::unordered_map<int, std::vector<ILightWithId *>> lights = LightColorizer::GetLightsPropagationGrouped(
                            lightSwitchEventEffect);
                    int lightCount = lights.size();

                    getLogger().debug("Prop id data is");
                    PrintJSONValue(propIDData);

                    if (propIDData.IsInt64() || propIDData.IsInt()) {
                        auto propIdLong = propIDData.GetInt();
                        if (lightCount > propIdLong) {
                            OverrideLightWithIdActivation = std::make_optional(lights[propIdLong]);
                        }
                    } else {
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

                        OverrideLightWithIdActivation = std::make_optional(overrideLights);
                    }
                }

                //gradientObject
                // FIXME, THIS IS VERY TEMP!!
                if (dynData->HasMember("_lightGradient")) {
                    color = ChromaGradientController::AddGradient(&dynData->FindMember("_lightGradient")->value,
                                                                  beatmapEventData->type, beatmapEventData->time);
                }
            }
        }

        std::optional<UnityEngine::Color> colorData = dynData ? ChromaUtils::ChromaUtilities::GetColorFromData(dynData)
                                                              : std::nullopt;
        if (colorData) {
            color = colorData;
            ChromaGradientController::CancelGradient(beatmapEventData->type);
        }
    }


    if (color) {
        LightColorizer::SetLightingColors(monobehaviour, color, color, color, color);
    } else if (!ChromaGradientController::IsGradientActive(beatmapEventData->type)) {
        LightColorizer::Reset(monobehaviour);
    }
}