#include "LightColorManager.hpp"
#include "Chroma.hpp"
#include "colorizer/LightColorizer.hpp"
#include "GlobalNamespace/LightSwitchEventEffect.hpp"
#include "LegacyLightHelper.hpp"
#include "utils/ChromaUtils.hpp"
#include "ChromaGradientController.hpp"

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

            // todo(nyamimi~): add support for _lightID.

            if(dynData.HasMember("_propID")) {
                int propID = dynData["_propID"].GetInt();
                auto lights = LightColorizer::GetLightsPropagationGrouped(lightSwitchEventEffect);
                int lightCount = lights.size();

                // propIDobjects??


                if (lightCount > propID) {
                    OverrideLightWithIdActivation = lights[propID];
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