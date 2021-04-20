#include "ChromaConfig.hpp"
#include "hooks/SceneTransition/SceneTransitionHelper.hpp"
#include <vector>
#include <optional>

#include "lighting/LegacyLightHelper.hpp"
#include "ChromaController.hpp"
#include "lighting/LightIDTableManager.hpp"

#include "GlobalNamespace/BeatmapDataSO.hpp"
#include "GlobalNamespace/BeatmapEnvironmentHelper.hpp"
#include "GlobalNamespace/EnvironmentInfoSO.hpp"
#include "Chroma.hpp"

#include "utils/ChromaUtils.hpp"

#include <cstdlib>

using namespace CustomJSONData;
using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;

void SceneTransitionHelper::Patch(GlobalNamespace::IDifficultyBeatmap* customBeatmapData) {
    SceneTransitionHelper::BasicPatch(customBeatmapData);
}

void SceneTransitionHelper::Patch(GlobalNamespace::IDifficultyBeatmap* customBeatmapData, OverrideEnvironmentSettings*& overrideEnvironmentSettings) {


    bool chromaRequirement = SceneTransitionHelper::BasicPatch(customBeatmapData);
    if (chromaRequirement && getChromaConfig().environmentEnhancementsEnabled.GetValue() && customBeatmapData)
        //TODO: Actually implement this once info.dat information is retrievable
        // && customBeatmapData->customData && customBeatmapData->customData->value && customBeatmapData->customData->value->HasMember("_environmentRemoval"))
    {
        auto il2cppObject = reinterpret_cast<Il2CppObject*>(customBeatmapData);
        getLogger().debug("Beatmap klass: %s", il2cpp_utils::ClassStandardName(il2cppObject->klass).c_str());
        if (ASSIGNMENT_CHECK(classof(CustomJSONData::CustomBeatmapData*), il2cppObject->klass)) {
            auto customBeatmapDataCustom = reinterpret_cast<CustomJSONData::CustomBeatmapData *>(customBeatmapData);

            if (customBeatmapDataCustom->customData) {
                auto dynData = customBeatmapDataCustom->customData->value;

                if (dynData) {
                    auto it = dynData.value().get().FindMember(ENVIRONMENT);
                    auto it2 = dynData.value().get().FindMember(ENVIRONMENTREMOVAL);

                    if (it != dynData.value().get().MemberEnd() || it2 != dynData.value().get().MemberEnd()) {
                        overrideEnvironmentSettings = nullptr;
                    }
                }
            }
        }
    }
}

bool SceneTransitionHelper::BasicPatch(GlobalNamespace::IDifficultyBeatmap* customBeatmapData) {
    auto environmentInfo = BeatmapEnvironmentHelper::GetEnvironmentInfo(customBeatmapData);

    LightIDTableManager::SetEnvironment(to_utf8(csstrtostr(environmentInfo->serializedName)));

    // please let me remove this shit
    bool legacyOverride = false;



    auto beatmapEvents = customBeatmapData->get_beatmapData()->beatmapEventsData->items;
    auto length = beatmapEvents->Length();
    getLogger().debug("Checking at most %d for ChromaLite notes", length);
    for (int i = 0; i < length; i++) {
        auto event = beatmapEvents->values[i];

        if (!event) continue;

        if (event->value >= LegacyLightHelper::RGB_INT_OFFSET)
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