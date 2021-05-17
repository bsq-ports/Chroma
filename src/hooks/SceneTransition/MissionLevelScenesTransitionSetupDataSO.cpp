#include "Chroma.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/MissionLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/OverrideEnvironmentSettings.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
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

MAKE_HOOK_OFFSETLESS(MissionLevelScenesTransitionSetupDataSO_Init,void,MissionLevelScenesTransitionSetupDataSO* self,
                     Il2CppString* missionId, GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap,
                     GlobalNamespace::IPreviewBeatmapLevel* previewBeatmapLevel, Array<GlobalNamespace::MissionObjective*>* missionObjectives,
                     GlobalNamespace::ColorScheme* overrideColorScheme, GlobalNamespace::GameplayModifiers* gameplayModifiers,
                     GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings,
                     Il2CppString* backButtonText) {
    MissionLevelScenesTransitionSetupDataSO_Init(self, missionId, difficultyBeatmap, previewBeatmapLevel, missionObjectives,
                                                 overrideColorScheme, gameplayModifiers,
                                                 playerSpecificSettings, backButtonText);
    SceneTransitionHelper::Patch(difficultyBeatmap);
}

void Chroma::Hooks::MissionLevelScenesTransitionSetupDataSO() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), MissionLevelScenesTransitionSetupDataSO_Init, il2cpp_utils::FindMethodUnsafe("", "MissionLevelScenesTransitionSetupDataSO", "Init", 8));
}