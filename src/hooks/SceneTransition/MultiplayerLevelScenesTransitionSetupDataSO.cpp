#include "Chroma.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/MultiplayerLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/OverrideEnvironmentSettings.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/PracticeSettings.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/BeatmapLevelSO.hpp"
#include "colorizer/NoteColorizer.hpp"
#include "UnityEngine/Space.hpp"
#include "UnityEngine/Transform.hpp"

#include "hooks/SceneTransition/SceneTransitionHelper.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Chroma;

MAKE_HOOK_MATCH(MultiplayerLevelScenesTransitionSetupDataSO_Init, &MultiplayerLevelScenesTransitionSetupDataSO::Init,
                void, MultiplayerLevelScenesTransitionSetupDataSO* self, 
                StringW gameMode, 
                ByRef<BeatmapKey> beatmapKey,
                BeatmapLevel* beatmapLevel, 
                IBeatmapLevelData* beatmapLevelData, 
                ColorScheme* overrideColorScheme, 
                GameplayModifiers* gameplayModifiers, 
                PlayerSpecificSettings* playerSpecificSettings, 
                PracticeSettings* practiceSettings, 
                AudioClipAsyncLoader* audioClipAsyncLoader, 
                BeatmapDataLoader* beatmapDataLoader, 
                bool useTestNoteCutSoundEffects) {
  MultiplayerLevelScenesTransitionSetupDataSO_Init(
      self, gameMode, beatmapKey, beatmapLevel, beatmapLevelData, overrideColorScheme, gameplayModifiers, 
      playerSpecificSettings, practiceSettings, audioClipAsyncLoader, beatmapDataLoader, useTestNoteCutSoundEffects);
  //SceneTransitionHelper::Patch(difficultyBeatmap);
}

void MultiplayerLevelScenesTransitionSetupDataSOHook() {
  INSTALL_HOOK(ChromaLogger::Logger, MultiplayerLevelScenesTransitionSetupDataSO_Init);
}

ChromaInstallHooks(MultiplayerLevelScenesTransitionSetupDataSOHook)