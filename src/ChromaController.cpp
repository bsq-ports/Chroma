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

static bool hookInstalled = false;

MAKE_HOOK_OFFSETLESS(ChromaController_NoteCutEvent, void, BeatmapObjectManager *self, NoteController *noteController,
                     NoteCutInfo *noteCutInfo) {
    if (!ChromaController::DoChromaHooks()) {
        ChromaController_NoteCutEvent(self, noteController, noteCutInfo);
        return;
    }
    NoteColorizer::ColorizeSaber(noteController, noteCutInfo);
    ChromaController_NoteCutEvent(self, noteController, noteCutInfo);
}

custom_types::Helpers::Coroutine ChromaController::DelayedStartEnumerator(GlobalNamespace::BeatmapObjectSpawnController *beatmapObjectSpawnController) {
    co_yield reinterpret_cast<enumeratorT *>(CRASH_UNLESS(il2cpp_utils::New<UnityEngine::WaitForEndOfFrame *>()));

    Chroma::ChromaController::BeatmapObjectSpawnController = beatmapObjectSpawnController;
    auto *beatmapObjectManager = reinterpret_cast<BeatmapObjectManager *>(beatmapObjectSpawnController->beatmapObjectSpawner);
    BeatmapObjectCallbackController *coreSetup = reinterpret_cast<BeatmapObjectCallbackController *>(beatmapObjectSpawnController->beatmapObjectCallbackController);
    Chroma::ChromaController::IAudioTimeSource = coreSetup->audioTimeSource;

    IReadonlyBeatmapData *beatmapData = coreSetup->beatmapData;

    if (!hookInstalled) {
        INSTALL_HOOK_OFFSETLESS(getLogger(), ChromaController_NoteCutEvent,
                                il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectManager", "HandleNoteControllerNoteWasCut", 2));
        hookInstalled = true;
    }

    if (DoChromaHooks() && getChromaConfig().environmentEnhancementsEnabled.GetValue()) {
        auto customBeatmap = reinterpret_cast<CustomJSONData::CustomBeatmapData *>(beatmapData);
        EnvironmentEnhancementManager::Init(customBeatmap, beatmapObjectSpawnController->get_noteLinesDistance());
    }

    auto list = reinterpret_cast<Generic::List_1<BeatmapEventData *> *>(beatmapData->get_beatmapEventsData());
    std::vector<GlobalNamespace::BeatmapEventData*> eventData;

    for (int i = 0; i < list->items->Length(); i++) {
        eventData.emplace_back(list->items->values[i]);
    }

    // please let me kill legacy
    LegacyLightHelper::Activate(eventData);

    co_return; // Reached end of coroutine
}


void ChromaController::OnActiveSceneChanged(UnityEngine::SceneManagement::Scene current) {
    getLogger().debug("Clear scene");

    if (current.IsValid() && to_utf8(csstrtostr(current.get_name())) == "GameCore") {
        RingManagers.clear();
        LightColorizer::ClearLSEColorManagers();
        ObstacleColorizer::ClearOCColorManagers();
        BombColorizer::ClearBNCColorManagers();
        NoteColorizer::ClearCNVColorManagers();
        SaberColorizer::ClearBSMColorManagers();
    }
}

bool ChromaController::ChromaRequired() {
    // 1 is true
    auto reqVar = getenv("req_Chroma");
    auto sugVar = getenv("sug_Chroma");

    return true || (reqVar && strcmp(reqVar, "1") == 0) || (sugVar && strcmp(sugVar, "1") == 0);
}

bool ChromaController::DoColorizerSabers() {
    return getChromaConfig().customColorEventsEnabled.GetValue() && ChromaRequired();
}

bool ChromaController::_ChromaLegacy = false;
void ChromaController::SetChromaLegacy(bool v) {
    getLogger().debug("Set chroma legacy to %s", v ? "true" : "false");
    _ChromaLegacy = v;
}

bool ChromaController::GetChromaLegacy() {
    return _ChromaLegacy;
}


bool ChromaController::DoChromaHooks() {
    return getChromaConfig().customColorEventsEnabled.GetValue() && (ChromaRequired());
}