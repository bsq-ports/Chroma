#include "Chroma.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/MissionLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/OverrideEnvironmentSettings.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
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


MAKE_HOOK_MATCH(MissionLevelScenesTransitionSetupDataSO_Init, static_cast<void(MissionLevelScenesTransitionSetupDataSO::*)(StringW, ByRef<BeatmapKey>, BeatmapLevel*, ArrayW<MissionObjective*>, ColorScheme*, GameplayModifiers*, PlayerSpecificSettings*, EnvironmentsListModel*, BeatmapLevelsModel*, AudioClipAsyncLoader*, ::BeatmapDataLoader*, StringW)>(&MissionLevelScenesTransitionSetupDataSO::Init),
                    void, MissionLevelScenesTransitionSetupDataSO* self, 
                    StringW missionId, 
                    ByRef<BeatmapKey> beatmapKey,
                    BeatmapLevel* beatmapLevel, 
                    ArrayW<MissionObjective*> missionObjectives, 
                    ColorScheme* overrideColorScheme,
                    GameplayModifiers* gameplayModifiers, 
                    PlayerSpecificSettings* playerSpecificSettings, 
                    EnvironmentsListModel* environmentsListModel, 
                    BeatmapLevelsModel* beatmapLevelsModel, 
                    AudioClipAsyncLoader* audioClipAsyncLoader, 
                    BeatmapDataLoader* beatmapDataLoader, 
                    StringW backButtonText) {
  MissionLevelScenesTransitionSetupDataSO_Init(self, missionId, beatmapKey, beatmapLevel, missionObjectives, overrideColorScheme,
                                               gameplayModifiers, playerSpecificSettings, environmentsListModel, beatmapLevelsModel, 
                                               audioClipAsyncLoader, beatmapDataLoader, backButtonText);
  //SceneTransitionHelper::Patch(difficultyBeatmap);
}

void MissionLevelScenesTransitionSetupDataSOHook() {
  INSTALL_HOOK(ChromaLogger::Logger, MissionLevelScenesTransitionSetupDataSO_Init);
}

ChromaInstallHooks(MissionLevelScenesTransitionSetupDataSOHook)