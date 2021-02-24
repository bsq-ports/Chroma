#include "LightColorManager.hpp"
#include "Chroma.hpp"
#include "colorizer/LightColorizer.hpp"
#include "GlobalNamespace/LightSwitchEventEffect.hpp"
#include "LegacyLightHelper.hpp"
#include "utils/ChromaUtils.hpp"
#include "ChromaGradientController.hpp"
#include "hooks/LightSwitchEventEffect.hpp"

#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"


using namespace Chroma;
using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;

void Chroma::LightColorManager::ColorLightSwitch(MonoBehaviour* monobehaviour, CustomBeatmapEventData* beatmapEventData) {
    LightColorizer::SetLastValue(monobehaviour, beatmapEventData->value);

    std::optional<UnityEngine::Color> color;

    color = LegacyLightHelper::GetLegacyColor(beatmapEventData);

    if(beatmapEventData->customData) {
        rapidjson::Value &dynData = *beatmapEventData->customData;
        if (il2cpp_functions::class_is_assignable_from(monobehaviour->klass, classof(LightSwitchEventEffect *))) {
            auto *lightSwitchEventEffect = reinterpret_cast<LightSwitchEventEffect *>(monobehaviour);

            if(dynData.HasMember("_lightID")) {
                rapidjson::Value &lightIdData = dynData["_lightID"];
                std::vector<ILightWithId *> lights = LightColorizer::GetLights(lightSwitchEventEffect);
                int lightCount = lights.size();

                if (lightIdData.IsInt64()) {
                    auto lightIdLong = lightIdData.GetInt64();
                    if (lightCount > lightIdLong) {
                        std::vector<ILightWithId*> overrideLights;

                        overrideLights.push_back(lights[lightIdLong]);

                        OverrideLightWithIdActivation = std::make_optional(overrideLights);
                    }
                } else {
                    auto lightIDobjects = lightIdData.GetObject();
                    std::vector<int> lightIDArray;


                    for (auto& lightId : lightIDobjects) {
                        lightIDArray.push_back(lightId.value.GetInt());
                    }

                    std::vector<ILightWithId*> overrideLights;

                    for (auto lightId : lightIDArray) {
                        if (lightCount > lightId) {
                            overrideLights.push_back(lights[lightId]);
                        }
                    }

                    OverrideLightWithIdActivation = std::make_optional(overrideLights);
                }
            }

            if(dynData.HasMember("_propID")) {
                rapidjson::Value &propIDData = dynData["_propID"];
                std::unordered_map<int, std::vector<ILightWithId *>> lights = LightColorizer::GetLightsPropagationGrouped(lightSwitchEventEffect);
                int lightCount = lights.size();

                if (propIDData.IsInt64()) {
                    auto propIdLong = propIDData.GetInt64();
                    if (lightCount > propIdLong) {
                        OverrideLightWithIdActivation = std::make_optional(lights[propIdLong]);
                    }
                } else {
                    auto propIDobjects = propIDData.GetObject();
                    std::vector<int> propIDArray;


                    for (auto& lightId : propIDobjects) {
                        propIDArray.push_back(lightId.value.GetInt());
                    }

                    std::vector<ILightWithId*> overrideLights;

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
            if(dynData.HasMember("_lightGradient")) {
                color = ChromaGradientController::AddGradient(&dynData["_lightGradient"], beatmapEventData->type, beatmapEventData->time);
            }
        }

        std::optional<UnityEngine::Color> colorData = ChromaUtils::ChromaUtilities::GetColorFromData(&dynData);
        if (colorData){
            color = colorData;
            ChromaGradientController::CancelGradient(beatmapEventData->type);
        }
    }

    if (color){
        LightColorizer::SetLightingColors(monobehaviour, color, color, color, color);
    } else if (!ChromaGradientController::IsGradientActive(beatmapEventData->type)){
        LightColorizer::Reset(monobehaviour);
    }
}