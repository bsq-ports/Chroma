#pragma once

#include "Chroma.hpp"
#include "colorizer/LightColorizer.hpp"
#include "lighting/LegacyLightHelper.hpp"
#include "utils/ChromaUtils.hpp"
#include "lighting/ChromaGradientController.hpp"

#include "lighting/ChromaEventData.hpp"
#include "hooks/LightSwitchEventEffect.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "UnityEngine/MonoBehaviour.hpp"

namespace Chroma {
    class LightColorManager {
        static void SetLegacyPropIdOverride(std::vector<GlobalNamespace::ILightWithId *> lights) {
            LightSwitchEventEffectHolder::LegacyLightOverride = std::make_optional(lights);
        }

    public:
        static void ColorLightSwitch(GlobalNamespace::BeatmapEventData *beatmapEventData) {
            using namespace CustomJSONData;
            using namespace GlobalNamespace;
            using namespace UnityEngine;

            static auto contextLogger = getLogger().WithContext(ChromaLogger::ColorLightSwitch);

            auto chromaIt = ChromaEventDataManager::ChromaEventDatas.find(beatmapEventData);


            // Aero thinks legacy was a mistake. I think a Quest port was a bigger mistake.
            std::optional<Sombrero::FastColor> color;

            if (chromaIt == ChromaEventDataManager::ChromaEventDatas.end()) {
                color = LegacyLightHelper::GetLegacyColor(beatmapEventData);
                if (color == std::nullopt)
                    return;
            } else {


                debugSpamLog(contextLogger, "Color is legacy? %s", color ? "true" : "false");


                auto const chromaData = chromaIt->second;

                auto const lightMember = chromaData->LightID;
                if (lightMember) {
                    rapidjson::Value const &lightIdData = *lightMember;

                    if (lightIdData.IsNumber()) {
                        auto lightIdLong = lightIdData.GetInt64();
                        LightSwitchEventEffectHolder::LightIDOverride = std::make_optional(
                                std::vector<int>{(int) lightIdLong});
                    } else if (lightIdData.IsObject() || lightIdData.IsArray()) {
                        // It's a object
                        std::vector<int> lightIDArray;

                        if (lightIdData.IsObject()) {
                            auto const &lightIDobjects = lightIdData.GetObject();

                            lightIDArray.reserve(lightIDobjects.MemberCount());
                            for (auto &lightId: lightIDobjects) {
                                lightIDArray.push_back(lightId.value.GetInt());
                            }
                        } else if (lightIdData.IsArray()) {
                            // It's a list
                            auto const &lightIDobjects = lightIdData.GetArray();

                            lightIDArray.reserve(lightIDobjects.Size());

                            for (auto &lightId: lightIDobjects) {
                                lightIDArray.push_back(lightId.GetInt());
                            }
                        }

                        LightSwitchEventEffectHolder::LightIDOverride = std::make_optional(lightIDArray);
                    }
                }


                // Prop ID is deprecated apparently.  https://github.com/Aeroluna/Chroma/commit/711cb19f7d03a1776a24cef52fd8ef6fd7685a2b#diff-b8fcfff3ebc4ceb7b43d8401d9f50750dc88326d0a87897c5593923e55b23879R41
                auto propMember = chromaData->PropID;
                if (propMember) {
                    rapidjson::Value const &propIDData = *propMember;

                    std::unordered_map<int, std::vector<ILightWithId *>> lights = LightColorizer::GetLightColorizer(
                            beatmapEventData->type)->LightsPropagationGrouped;
                    int lightCount = (int) lights.size();


                    if (propIDData.IsNumber()) {
                        auto propIdLong = propIDData.GetInt();

                        if (lightCount > propIdLong) {
                            SetLegacyPropIdOverride(lights[propIdLong]);
                        }
                    } else {
                        // It's a list
                        auto const &propIDobjects = propIDData.GetObject();

                        std::vector<ILightWithId *> overrideLights;

                        for (auto const &lightId: propIDobjects) {
                            int propId = lightId.value.GetInt();
                            if (lightCount > propId) {
                                for (auto &l: lights[propId]) {
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


                std::optional<Sombrero::FastColor> const &colorData = chromaData->ColorData;
                if (colorData) {
                    color = colorData;
                    ChromaGradientController::CancelGradient(beatmapEventData->type);
                }
            }


            if (color) {
                LightColorizer::ColorizeLight(beatmapEventData->type, false, {*color, *color, *color, *color});
            } else if (!ChromaGradientController::IsGradientActive(beatmapEventData->type)) {
                LightColorizer::ColorizeLight(beatmapEventData->type, false,
                                              {std::nullopt, std::nullopt, std::nullopt, std::nullopt});
            }
        }
    };
}