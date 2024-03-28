#include "Chroma.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/OverrideEnvironmentSettings.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/PracticeSettings.hpp"
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
using namespace System;

MAKE_HOOK_MATCH(StandardLevelScenesTransitionSetupDataSO_Init, static_cast<void(StandardLevelScenesTransitionSetupDataSO::*)(StringW, ByRef<BeatmapKey>, BeatmapLevel*, OverrideEnvironmentSettings*, ColorScheme*, ColorScheme*, GameplayModifiers*, PlayerSpecificSettings*, PracticeSettings*, EnvironmentsListModel*, AudioClipAsyncLoader*, ::BeatmapDataLoader*, StringW, BeatmapLevelsModel*, bool, bool, Nullable_1<RecordingToolManager::SetupData>)>(&StandardLevelScenesTransitionSetupDataSO::Init),
                    void, StandardLevelScenesTransitionSetupDataSO* self, 
                    StringW gameMode, 
                    ByRef<BeatmapKey> beatmapKey,
                    BeatmapLevel* beatmapLevel, 
                    OverrideEnvironmentSettings* overrideEnvironmentSettings, 
                    ColorScheme* overrideColorScheme, 
                    ColorScheme* beatmapOverrideColorScheme, 
                    GameplayModifiers* gameplayModifiers, 
                    PlayerSpecificSettings* playerSpecificSettings, 
                    PracticeSettings* practiceSettings, 
                    EnvironmentsListModel* environmentsListModel, 
                    AudioClipAsyncLoader* audioClipAsyncLoader, 
                    BeatmapDataLoader* beatmapDataLoader, 
                    StringW backButtonText,
                    BeatmapLevelsModel* beatmapLevelsModel, 
                    bool useTestNoteCutSoundEffects, bool startPaused, 
                    Nullable_1<RecordingToolManager::SetupData> recordingToolData) {
  auto customBeatmapLevel = il2cpp_utils::try_cast<SongCore::SongLoader::CustomBeatmapLevel>(beatmapLevel);
  if(!customBeatmapLevel) {
    StandardLevelScenesTransitionSetupDataSO_Init(
      self, gameMode, beatmapKey, beatmapLevel, overrideEnvironmentSettings, overrideColorScheme, beatmapOverrideColorScheme, gameplayModifiers,
      playerSpecificSettings, practiceSettings, environmentsListModel, audioClipAsyncLoader, beatmapDataLoader, backButtonText, beatmapLevelsModel, useTestNoteCutSoundEffects, startPaused, recordingToolData);
  }

  auto* settings = overrideEnvironmentSettings;
  self->set_gameMode(gameMode);
  self->set_beatmapKey(beatmapKey.heldRef);
  self->set_beatmapLevel(beatmapLevel);
  self->set_practiceSettings(practiceSettings);
  self->set_gameplayModifiers(gameplayModifiers);
  self->InitEnvironmentInfo(overrideEnvironmentSettings, environmentsListModel);  

  SceneTransitionHelper::Patch(customBeatmapLevel.value(), beatmapKey.heldRef, self->get_environmentInfo().ptr(), settings);

  StandardLevelScenesTransitionSetupDataSO_Init(
      self, gameMode, beatmapKey, beatmapLevel, overrideEnvironmentSettings, overrideColorScheme, beatmapOverrideColorScheme, gameplayModifiers,
      playerSpecificSettings, practiceSettings, environmentsListModel, audioClipAsyncLoader, beatmapDataLoader, backButtonText, beatmapLevelsModel, useTestNoteCutSoundEffects, startPaused, recordingToolData);
}

void StandardLevelScenesTransitionSetupDataSOHook() {
  INSTALL_HOOK(ChromaLogger::Logger, StandardLevelScenesTransitionSetupDataSO_Init);
}

ChromaInstallHooks(StandardLevelScenesTransitionSetupDataSOHook)