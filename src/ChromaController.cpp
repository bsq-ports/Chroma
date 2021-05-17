#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "GlobalNamespace/BeatmapObjectManager.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"
#include "GlobalNamespace/BeatmapObjectCallbackController.hpp"
#include "GlobalNamespace/IBeatmapObjectCallbackController.hpp"
#include "GlobalNamespace/BeatmapLineData.hpp"
#include "GlobalNamespace/BeatmapObjectType.hpp"
#include "GlobalNamespace/BeatmapObjectData.hpp"
#include "GlobalNamespace/Saber.hpp"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/GameObject.hpp"

#include "System/Action.hpp"
#include "ChromaConfig.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"

#include "main.hpp"
#include "colorizer/NoteColorizer.hpp"
#include "colorizer/SaberColorizer.hpp"
#include "colorizer/ObstacleColorizer.hpp"
#include "colorizer/BombColorizer.hpp"
#include "colorizer/LightColorizer.hpp"
#include "lighting/LegacyLightHelper.hpp"
#include "lighting/environment_enhancements/EnvironmentEnhancementManager.hpp"
#include "hooks/TrackLaneRingsManager.hpp"

#include "custom-types/shared/coroutine.hpp"

#include <vector>

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;
using namespace custom_types::Helpers;

bool ChromaController::ChromaLegacy = false;
bool ChromaController::TutorialMode = false;



custom_types::Helpers::Coroutine ChromaController::DelayedStartEnumerator(GlobalNamespace::BeatmapObjectSpawnController *beatmapObjectSpawnController) {
    co_yield reinterpret_cast<enumeratorT *>(CRASH_UNLESS(il2cpp_utils::New<UnityEngine::WaitForEndOfFrame *>()));

    Chroma::ChromaController::BeatmapObjectSpawnController = beatmapObjectSpawnController;
    BeatmapObjectCallbackController *coreSetup = il2cpp_utils::cast<BeatmapObjectCallbackController>(beatmapObjectSpawnController->beatmapObjectCallbackController);
    Chroma::ChromaController::IAudioTimeSource = coreSetup->audioTimeSource;

    IReadonlyBeatmapData *beatmapData = coreSetup->beatmapData;

    if (DoChromaHooks() && getChromaConfig().environmentEnhancementsEnabled.GetValue()) {
        auto customBeatmap = il2cpp_utils::cast<CustomJSONData::CustomBeatmapData>(beatmapData);
        EnvironmentEnhancementManager::Init(customBeatmap, beatmapObjectSpawnController->get_noteLinesDistance());
    }

    auto list = il2cpp_utils::cast<Generic::List_1<BeatmapEventData *>>(beatmapData->get_beatmapEventsData());
    std::vector<GlobalNamespace::BeatmapEventData*> eventData;
    list->items->copy_to(eventData);

    // please let me kill legacy
    LegacyLightHelper::Activate(eventData);

    co_return; // Reached end of coroutine
}


void ChromaController::OnActiveSceneChanged(UnityEngine::SceneManagement::Scene current) {
    getLogger().debug("Clear scene");

    if (current && current.IsValid() && to_utf8(csstrtostr(current.get_name())) == "GameCore") {
        TrackLaneRingsManagerHolder::RingManagers.clear();
        LightColorizer::ClearLSEColorManagers();
        ObstacleColorizer::ClearOCColorManagers();
        BombColorizer::ClearBNCColorManagers();
        NoteColorizer::ClearCNVColorManagers();
        SaberColorizer::ClearBSMColorManagers();
    }
}

bool ChromaController::ChromaRequired() {
    // 1 is true
    char* reqVar = getenv("req_Chroma");
    char* sugVar = getenv("sug_Chroma");

    return !TutorialMode && (true || (reqVar && std::string(reqVar) == "1") || (sugVar && std::string(sugVar) == "1"));
}

bool ChromaController::DoColorizerSabers() {
    return getChromaConfig().customColorEventsEnabled.GetValue() && ChromaRequired();
}

void ChromaController::SetChromaLegacy(bool v) {
    getLogger().debug("Set chroma legacy to %s", v ? "true" : "false");
    ChromaLegacy = v;
}

bool ChromaController::GetChromaLegacy() {
    return ChromaLegacy;
}


bool ChromaController::DoChromaHooks() {
    return getChromaConfig().customColorEventsEnabled.GetValue() && (ChromaRequired());
}