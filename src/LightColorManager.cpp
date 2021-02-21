#include "LightColorManager.hpp"
#include "Chroma.hpp"
#include "colorizer/LightColorizer.hpp"
#include "GlobalNamespace/LightSwitchEventEffect.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;

void Chroma::LightColorManager::ColorLightSwitch(MonoBehaviour* monobehaviour, CustomBeatmapEventData* beatmapEventData) {
    LightColorizer::SetLastValue(monobehaviour, beatmapEventData->value);

    UnityEngine::Color color;
    bool colorWasTouched = false;

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
            if(dynData.HasMember("_lightGradient")&&dynData["_lightGradient"].HasMember("_startColor")) {
                auto col = dynData["_lightGradient"]["_startColor"].GetArray();
                float r = col[0].GetFloat();
                float g = col[1].GetFloat();
                float b = col[2].GetFloat();
                // not parsing "a" atm..
                color.r = r;
                color.g = g;
                color.b = b;
                color.a = 1;
                colorWasTouched=true;
            }
        }

        // don't be bri'ish innit.
        if(dynData.HasMember("_color")) {
            auto col = dynData["_color"].GetArray();
            float r = col[0].GetFloat();
            float g = col[1].GetFloat();
            float b = col[2].GetFloat();
            // not parsing "a" atm..
            color.r = r;
            color.g = g;
            color.b = b;
            color.a = 1;
            colorWasTouched=true;
        }
    }

    if (colorWasTouched){
        LightColorizer::SetLightingColors(monobehaviour, std::make_optional(color), std::make_optional(color), std::make_optional(color), std::make_optional(color));
    }
}