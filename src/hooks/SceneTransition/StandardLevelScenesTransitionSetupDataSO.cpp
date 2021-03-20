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
#include "UnityEngine/Vector3.hpp"
#include "GlobalNamespace/BeatmapLevelSO.hpp"
#include "colorizer/NoteColorizer.hpp"
#include "UnityEngine/Random.hpp"
#include "UnityEngine/Space.hpp"
#include "UnityEngine/Transform.hpp"

#include "hooks/SceneTransition/SceneTransitionHelper.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Chroma;

MAKE_HOOK_OFFSETLESS(StandardLevelScenesTransitionSetupDataSO_Init,void,StandardLevelScenesTransitionSetupDataSO* self, ::Il2CppString* gameMode,
                     BeatmapLevelSO::DifficultyBeatmap* difficultyBeatmap,
                     GlobalNamespace::OverrideEnvironmentSettings* overrideEnvironmentSettings,
                     GlobalNamespace::ColorScheme* overrideColorScheme, GlobalNamespace::GameplayModifiers* gameplayModifiers,
                     GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings, GlobalNamespace::PracticeSettings* practiceSettings,
                     ::Il2CppString* backButtonText, bool useTestNoteCutSoundEffects) {
    auto settings = overrideEnvironmentSettings;
    SceneTransitionHelper::Patch(reinterpret_cast<IDifficultyBeatmap*>(difficultyBeatmap), settings);
    StandardLevelScenesTransitionSetupDataSO_Init(self, gameMode, difficultyBeatmap, settings,
                                                  overrideColorScheme, gameplayModifiers,
                                                  playerSpecificSettings, practiceSettings, backButtonText,
                                                  useTestNoteCutSoundEffects);
}

void Chroma::Hooks::StandardLevelScenesTransitionSetupDataSO() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), StandardLevelScenesTransitionSetupDataSO_Init, il2cpp_utils::FindMethodUnsafe("", "StandardLevelScenesTransitionSetupDataSO", "Init", 9));
}