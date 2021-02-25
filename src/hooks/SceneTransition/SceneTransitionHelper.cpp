#include "ChromaConfig.hpp"
#include "hooks/SceneTransition/SceneTransitionHelper.hpp"
#include <vector>
#include <optional>


#include "LegacyLightHelper.hpp"
#include "ChromaController.hpp"

#include <cstdlib>

using namespace CustomJSONData;
using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;

void SceneTransitionHelper::Patch(CustomBeatmapData *customBeatmapData) {
    SceneTransitionHelper::BasicPatch(customBeatmapData);
}

void SceneTransitionHelper::Patch(CustomBeatmapData* customBeatmapData, OverrideEnvironmentSettings*& overrideEnvironmentSettings) {
    bool chromaRequirement = SceneTransitionHelper::BasicPatch(customBeatmapData);
    if (chromaRequirement && getChromaConfig().environmentEnhancementsEnabled.GetValue() && customBeatmapData->customData->value->HasMember("_environmentRemoval")) {
        overrideEnvironmentSettings = nullptr;
    }
}

bool SceneTransitionHelper::BasicPatch(CustomBeatmapData* customBeatmapData) {
    auto& dynData = *customBeatmapData->customData->value;


//    bool lockPosition = dynData.HasMember("_lockPosition") ? dynData["_lockPosition"].GetBool() : false;

    auto& requirements = dynData.FindMember("_requirements")->value;
    auto& suggestions = dynData.FindMember("_suggestions")->value; // List<object>)Trees.at(customBeatmapData.beatmapCustomData, "_suggestions"))?.Cast<string>();



    bool chromaRequirement = (getenv("req_Chroma")) || (suggestions.HasMember("Chroma"));



    // please let me remove this shit
    bool legacyOverride;

    for (int i = 0; i < customBeatmapData->beatmapEventsData->items->Length(); i++) {
        if (customBeatmapData->beatmapEventsData->items->values[i]->value >=LegacyLightHelper::RGB_INT_OFFSET)
            legacyOverride = true;

        if (legacyOverride)
            break;
    }

    if (legacyOverride)
    {
        getLogger().warning("Legacy Chroma Detected...");
        getLogger().warning("Please do not use Legacy Chroma for new maps as it is deprecated and its functionality in future versions of Chroma cannot be guaranteed");
    }

//    ChromaController.ToggleChromaPatches((chromaRequirement || legacyOverride) && ChromaConfig.Instance.CustomColorEventsEnabled);

//    ChromaController::DoColorizerSabers = chromaRequirement && getChromaConfig().customColorEventsEnabled.GetValue();

    return chromaRequirement;
}

//void Chroma::Hooks::SceneTransitionHelper() {
//
//}