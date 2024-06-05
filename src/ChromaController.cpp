#include "main.hpp"
#include "ChromaConfig.hpp"
#include "ChromaLogger.hpp"
#include "ChromaController.hpp"

#include "GlobalNamespace/BeatmapObjectManager.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"
#include "UnityEngine/WaitForEndOfFrame.hpp"

#include "custom-types/shared/coroutine.hpp"

#include "beatsaber-hook/shared/utils/hooking.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"

#include "colorizer/NoteColorizer.hpp"
#include "colorizer/SaberColorizer.hpp"
#include "colorizer/ObstacleColorizer.hpp"
#include "colorizer/BombColorizer.hpp"
#include "colorizer/LightColorizer.hpp"
#include "colorizer/ParticleColorizer.hpp"

#include "lighting/LegacyLightHelper.hpp"
#include "environment_enhancements/EnvironmentEnhancementManager.hpp"
#include "environment_enhancements/MaterialsManager.hpp"
#include "lighting/ChromaGradientController.hpp"
#include "hooks/TrackLaneRingsManager.hpp"
#include "hooks/LightWithIdManager.hpp"

#include <vector>

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;
using namespace custom_types::Helpers;

bool ChromaController::ChromaLegacy = false;
bool ChromaController::ChromaMap = false;
bool ChromaController::TutorialMode = false;

std::unordered_set<std::string> ChromaController::ModsForcingDoHooks = std::unordered_set<std::string>();

custom_types::Helpers::Coroutine
ChromaController::DelayedStartEnumerator(GlobalNamespace::BeatmapObjectSpawnController* beatmapObjectSpawnController) {
  co_yield reinterpret_cast<enumeratorT>(CRASH_UNLESS(il2cpp_utils::New<UnityEngine::WaitForEndOfFrame*>()));

  PAPER_IL2CPP_CATCH_HANDLER(
      try {
        Chroma::ChromaController::BeatmapObjectSpawnController = beatmapObjectSpawnController;
        auto* coreSetup =
            il2cpp_utils::cast<BeatmapCallbacksController>(beatmapObjectSpawnController->_beatmapCallbacksController);
        Chroma::ChromaController::CallbacksController = coreSetup;

        IReadonlyBeatmapData* beatmapData = coreSetup->_beatmapData;

        if (!beatmapData) {
          ChromaLogger::Logger.debug("Beatmap is null, oh no what will we do now?");
          co_return;
          //            CRASH_UNLESS(beatmapData);
        }

        if (auto customBeatmap = il2cpp_utils::try_cast<CustomJSONData::CustomBeatmapData>(beatmapData)) {
          if (DoChromaHooks() && getChromaConfig().environmentEnhancementsEnabled.GetValue()) {
            // seriously what the fuck beat games
            // GradientBackground permanently yeeted because it looks awful and can ruin multi-colored chroma maps
            UnityEngine::GameObject* gradientBackground = GameObject::Find("/Environment/GradientBackground");
            if (gradientBackground) {
              gradientBackground->SetActive(false);
            }

            EnvironmentEnhancementManager::Init(*customBeatmap);
          }

          if (GetChromaLegacy()) {
            try {
              auto list = customBeatmap.value()->GetBeatmapItemsCpp<BasicBeatmapEventData*>(
                  BeatmapDataItem::BeatmapDataItemType::BeatmapEvent);

              // please let me kill legacy
              LegacyLightHelper::Activate(list);
            } catch (const Il2CppException& e) {
              ChromaLogger::Logger.error("Unable to run legacy due to exception?");
            }
          }
        } else {
          ChromaLogger::Logger.debug(
              "Beatmap class {}",
              il2cpp_utils::ClassStandardName(reinterpret_cast<Il2CppObject*>(beatmapData)->klass).c_str());
        }
      } catch (std::exception& e) {
        ChromaLogger::Logger.debug("Chroma controller failed: {}!", e.what());
        throw;
      })

  co_return; // Reached end of coroutine
}

void ChromaController::OnActiveSceneChanged(UnityEngine::SceneManagement::Scene current) {
  ChromaLogger::Logger.debug("Clear scene");

  if (current.IsValid() && current.get_name() == "GameCore") {
    CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Clearing all data");
    ChromaGradientController::clearInstance();
    TrackLaneRingsManagerHolder::RingManagers.clear();
    TrackLaneRingsManagerHolder::RingManagers.shrink_to_fit(); // Deallocate unnecessary used memory
    LightColorizer::Reset();
    ObstacleColorizer::Reset();
    BombColorizer::Reset();
    NoteColorizer::Reset();
    SaberColorizer::Reset();
    ParticleColorizer::Reset();
    LightIdRegisterer::Reset();
    MaterialsManager::Reset();
  }
}

void ChromaController::SetChromaLegacy(bool v) {
  ChromaLogger::Logger.debug("Set chroma legacy to {}", v ? "true" : "false");
  ChromaLegacy = v && getChromaConfig().customColorEventsEnabled.GetValue();
}

void ChromaController::setChromaRequired(bool chromaMap) {
  ChromaMap = chromaMap && getChromaConfig().customColorEventsEnabled.GetValue();
  ChromaLogger::Logger.debug("Set chroma required/suggested to {}", ChromaMap ? "true" : "false");
}

void ChromaController::AddForceDoHooks(modloader::ModInfo const& otherModInfo) {
  ChromaLogger::Logger.info("Adding force do hooks, ID: {}", otherModInfo.id.c_str());
  ModsForcingDoHooks.emplace(otherModInfo.id);
}
void ChromaController::RemoveForceDoHooks(modloader::ModInfo const& otherModInfo) {
  ChromaLogger::Logger.info("Removing force do hooks, ID: {}", otherModInfo.id.c_str());
  ModsForcingDoHooks.erase(otherModInfo.id);
}
