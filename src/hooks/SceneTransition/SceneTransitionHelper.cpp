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
    // please let me remove this shit
    bool legacyOverride = false;

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

    ChromaController::SetChromaLegacy(legacyOverride);

//    ChromaController.ToggleChromaPatches((chromaRequirement || legacyOverride) && ChromaConfig.Instance.CustomColorEventsEnabled);


    return ChromaController::ChromaRequired();
}

//void Chroma::Hooks::SceneTransitionHelper() {
//
//}