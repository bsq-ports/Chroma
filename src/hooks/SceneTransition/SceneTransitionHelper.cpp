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
    if (chromaRequirement && getChromaConfig().environmentEnhancementsEnabled.GetValue() &&
    customBeatmapData->customData && customBeatmapData->customData->value && customBeatmapData->customData->value->HasMember("_environmentRemoval")) {
        overrideEnvironmentSettings = nullptr;
    }
}

bool SceneTransitionHelper::BasicPatch(BeatmapData* customBeatmapData) {
    // please let me remove this shit
    bool legacyOverride = false;

    auto beatmapEvents = customBeatmapData->beatmapEventsData->items;
    auto length = beatmapEvents->Length();
    getLogger().debug("Basic patch %d", length);
    getLogger().debug("Beatmap class %s", customBeatmapData->klass->name);
    for (int i = 0; i < length; i++) {
        auto event = beatmapEvents->values[i];

        getLogger().debug("Event %p", event);

        // TODO: Event->value is causing acrash
        if (!event || !il2cpp_functions::class_is_assignable_from(event->klass, classof(BeatmapEventData*))) continue;

        getLogger().debug("Event klass %s", event->klass->name);
        getLogger().debug("Event value %d", event->value);
        if (event != nullptr && event->value >= LegacyLightHelper::RGB_INT_OFFSET)
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