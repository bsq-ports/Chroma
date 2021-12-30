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
                    auto const &lightIdData = *lightMember;
                    LightSwitchEventEffectHolder::LightIDOverride = lightIdData;
                }



                // Prop ID is deprecated apparently.  https://github.com/Aeroluna/Chroma/commit/711cb19f7d03a1776a24cef52fd8ef6fd7685a2b#diff-b8fcfff3ebc4ceb7b43d8401d9f50750dc88326d0a87897c5593923e55b23879R41
                auto propMember = chromaData->PropID;
                if (propMember) {
                    auto const &propIDData = *propMember;

                    auto const &lights = LightColorizer::GetLightColorizer(beatmapEventData->type)->LightsPropagationGrouped;
                    auto lightCount = (int) lights.size();

                    std::vector<ILightWithId *> overrideLights;

                    for (auto const &propId: propIDData) {
                        if (lightCount > propId) {
                            for (auto &l: lights.at(propId)) {
                                overrideLights.push_back(l);
                            }
                        }
                    }

                    SetLegacyPropIdOverride(overrideLights);
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