#include "LightAPI.hpp"
#include "colorizer/LightColorizer.hpp"

#include "conditional-dependencies/shared/main.hpp"
#include "shared/utils.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;


// TODO: unsure of this

EXPOSE_API(getLightColorSafe, LightAPI::LSEData*, MonoBehaviour* mb) {

    auto lse = LSEColorManager::GetLSEColorManager(mb);

    if (!lse) return nullptr;

    auto _lightColor0 = lse->_lightColor0->color;
    auto _lightColor1 = lse->_lightColor1->color;
    auto _lightColor0Boost = lse->_lightColor0Boost->color;
    auto _lightColor1Boost = lse->_lightColor1Boost->color;

    auto lseData = new LightAPI::LSEData {
        std::make_optional(_lightColor0),
        std::make_optional(_lightColor1),
        std::make_optional(_lightColor0Boost),
        std::make_optional(_lightColor1Boost)
    };

    return lseData;
}

EXPOSE_API(setLightColorSafe, void, MonoBehaviour* mb, std::optional<LightAPI::LSEData> lseData) {
    if (lseData)
        LightColorizer::SetLightingColors(mb, lseData->_lightColor0, lseData->_lightColor1, lseData->_lightColor0Boost, lseData->_lightColor1Boost);
    else
        LightColorizer::Reset(mb);
}


EXPOSE_API(SetAllLightingColors, void, std::optional<LightAPI::LSEData> data) {
    if (data) {
        LightColorizer::SetAllLightingColors(data.value()._lightColor0, data.value()._lightColor1, data.value()._lightColor0Boost,
                                             data.value()._lightColor1Boost);
    } else {
        LightColorizer::ResetAllLightingColors();
    }
}


// Returns array of ILightWithId*

// While it does create a pointer on the heap,
// the API side of this should move the elements back into a value type then delete this pointer.
extern "C" std::unordered_map<int, GlobalNamespace::ILightWithId *>* getLightsSafe(GlobalNamespace::LightSwitchEventEffect *lse) {
    auto vectorOrg = LightColorizer::GetLights(lse);
    auto vectorPtr = new std::unordered_map<int, GlobalNamespace::ILightWithId *>(std::move(vectorOrg));

    return vectorPtr;
}

// Returns 2d array of ILightWithId*
// While it does create a pointer on the heap,
// the API side of this should move the elements back into a value type then delete this pointer.

//EXPOSE_API(, std::unordered_map<int, std::vector<GlobalNamespace::ILightWithId *>>, GlobalNamespace::LightSwitchEventEffect *lse) {
extern "C" std::unordered_map<int, std::vector<GlobalNamespace::ILightWithId *>>* getLightsPropagationGroupedSafe(GlobalNamespace::LightSwitchEventEffect *lse) {
    auto mapOrg = LightColorizer::GetLightsPropagationGrouped(lse);
    auto mapPtr = new std::unordered_map<int, std::vector<GlobalNamespace::ILightWithId *>>(std::move(mapOrg));

    return mapPtr;
}