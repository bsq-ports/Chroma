#include "Chroma.hpp"
#include "lighting/LightColorManager.hpp"
#include "colorizer/LightColorizer.hpp"
#include "lighting/LegacyLightHelper.hpp"
#include "utils/ChromaUtils.hpp"
#include "lighting/ChromaGradientController.hpp"
#include "hooks/LightSwitchEventEffect.hpp"
#include "lighting/ChromaEventData.hpp"

using namespace Chroma;
using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;

void SetLegacyPropIdOverride(std::vector<ILightWithId*> lights) {
    LightSwitchEventEffectHolder::LegacyLightOverride = std::make_optional(lights);
}

void Chroma::LightColorManager::ColorLightSwitch(BeatmapEventData* beatmapEventData) {
    static auto contextLogger = getLogger().WithContext(ChromaLogger::ColorLightSwitch);

    auto chromaIt = ChromaEventDataManager::ChromaEventDatas.find(beatmapEventData);


    if (chromaIt == ChromaEventDataManager::ChromaEventDatas.end()) {
        return;
    }

    // Aero thinks legacy was a mistake. I think a Quest port was a bigger mistake.
    std::optional<UnityEngine::Color> color = LegacyLightHelper::GetLegacyColor(beatmapEventData);

    debugSpamLog(contextLogger, "Color is legacy? %s", color ? "true" : "false");


    auto chromaData = chromaIt->second;

    auto lightMember = chromaData->LightID;
    if (lightMember) {
        debugSpamLog(contextLogger, "JSON data 2");
        rapidjson::Value &lightIdData = *lightMember;

        if (lightIdData.IsInt() || lightIdData.IsInt64() || lightIdData.IsUint() || lightIdData.IsUint64()) {
            auto lightIdLong = lightIdData.GetInt64();
            debugSpamLog(contextLogger, "LightID int %d", lightIdLong);
            LightSwitchEventEffectHolder::LightIDOverride = std::make_optional(std::vector<int>{(int) lightIdLong});
        } else if (lightIdData.IsObject()) {
            // It's a object
            auto lightIDobjects = lightIdData.GetObject();
            std::vector<int> lightIDArray;
            lightIDArray.reserve(lightIDobjects.MemberCount());

            debugSpamLog(contextLogger, "LightID object:");

            PrintJSONValue(lightIdData);

            for (auto &lightId : lightIDobjects) {
                lightIDArray.push_back(lightId.value.GetInt());
            }

            LightSwitchEventEffectHolder::LightIDOverride = std::make_optional(lightIDArray);
        } else if (lightIdData.IsArray()) {
            // It's a list
            auto lightIDobjects = lightIdData.GetArray();
            std::vector<int> lightIDArray;
            lightIDArray.reserve(lightIDobjects.Size());

            debugSpamLog(contextLogger, "LightID array:");

            PrintJSONValue(lightIdData);

            for (auto &lightId : lightIDobjects) {
                lightIDArray.push_back(lightId.GetInt());
            }

            LightSwitchEventEffectHolder::LightIDOverride = std::make_optional(lightIDArray);
        }
    }


    // Prop ID is deprecated apparently.  https://github.com/Aeroluna/Chroma/commit/711cb19f7d03a1776a24cef52fd8ef6fd7685a2b#diff-b8fcfff3ebc4ceb7b43d8401d9f50750dc88326d0a87897c5593923e55b23879R41
    auto propMember = chromaData->PropID;
    if (propMember) {
        debugSpamLog(contextLogger, "JSON data 3");
        rapidjson::Value &propIDData = *propMember;

        std::unordered_map<int, std::vector<ILightWithId *>> lights = LightColorizer::GetLightColorizer(
                beatmapEventData->type)->LightsPropagationGrouped;
        int lightCount = (int) lights.size();

        debugSpamLog(contextLogger, "Prop id data is");
        PrintJSONValue(propIDData);

        if (propIDData.IsInt64() || propIDData.IsInt() || propIDData.IsUint() || propIDData.IsUint64()) {
            auto propIdLong = propIDData.GetInt();
            debugSpamLog(contextLogger, "It is an int prop %d %d", lightCount, propIdLong);
            if (lightCount > propIdLong) {
                SetLegacyPropIdOverride(lights[propIdLong]);
            }
        } else {
            debugSpamLog(contextLogger, "It is a list prop");
            // It's a list
            auto propIDobjects = propIDData.GetObject();

            std::vector<ILightWithId *> overrideLights;

            for (auto &lightId : propIDobjects) {
                int propId = lightId.value.GetInt();
                if (lightCount > propId) {
                    for (auto l : lights[propId]) {
                        overrideLights.push_back(l);
                    }
                }
            }

            SetLegacyPropIdOverride(overrideLights);
        }
    }


    auto gradient = chromaData->GradientObject;
    if (gradient) {
        color = ChromaGradientController::AddGradient(gradient.value(), beatmapEventData->type,
                                                      beatmapEventData->time);
    }


    std::optional<UnityEngine::Color> colorData = chromaData->ColorData;
    if (colorData) {
        color = colorData;
        ChromaGradientController::CancelGradient(beatmapEventData->type);
    }


    if (color) {
        LightColorizer::ColorizeLight(beatmapEventData->type, false, {*color, *color, *color, *color});
    } else if (!ChromaGradientController::IsGradientActive(beatmapEventData->type)) {
        LightColorizer::ColorizeLight(beatmapEventData->type, false,
                                      {std::nullopt, std::nullopt, std::nullopt, std::nullopt});
    }
}

