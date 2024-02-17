#include "Chroma.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/OverrideEnvironmentSettings.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/PracticeSettings.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/RecordingToolManager.hpp"
#include "GlobalNamespace/BeatmapLevelSO.hpp"
#include "colorizer/NoteColorizer.hpp"
#include "UnityEngine/Space.hpp"
#include "UnityEngine/Transform.hpp"

#include "hooks/SceneTransition/SceneTransitionHelper.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Chroma;

MAKE_HOOK_MATCH(StandardLevelScenesTransitionSetupDataSO_Init, &StandardLevelScenesTransitionSetupDataSO::Init, void,
                StandardLevelScenesTransitionSetupDataSO* self, StringW gameMode,
                ::GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap,
                ::GlobalNamespace::IPreviewBeatmapLevel* previewBeatmapLevel,
                ::GlobalNamespace::OverrideEnvironmentSettings* overrideEnvironmentSettings,
                ::GlobalNamespace::ColorScheme* overrideColorScheme,
                ::GlobalNamespace::ColorScheme* beatmapOverrideColorScheme,
                ::GlobalNamespace::GameplayModifiers* gameplayModifiers,
                ::GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings,
                ::GlobalNamespace::PracticeSettings* practiceSettings, ::StringW backButtonText,
                bool useTestNoteCutSoundEffects, bool startPaused, BeatmapDataCache* cache,
                System::Nullable_1<GlobalNamespace::RecordingToolManager::SetupData> recordingToolManager) {
  auto* settings = overrideEnvironmentSettings;
  SceneTransitionHelper::Patch(difficultyBeatmap, settings);
  StandardLevelScenesTransitionSetupDataSO_Init(
      self, gameMode, difficultyBeatmap, previewBeatmapLevel, settings, overrideColorScheme, beatmapOverrideColorScheme, gameplayModifiers,
      playerSpecificSettings, practiceSettings, backButtonText, useTestNoteCutSoundEffects, startPaused, cache, recordingToolManager);
}

void StandardLevelScenesTransitionSetupDataSOHook(Logger& logger) {
  INSTALL_HOOK(logger, StandardLevelScenesTransitionSetupDataSO_Init);
}

ChromaInstallHooks(StandardLevelScenesTransitionSetupDataSOHook)