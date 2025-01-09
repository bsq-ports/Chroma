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

MAKE_HOOK_MATCH(StandardLevelScenesTransitionSetupDataSO_Init,
                &StandardLevelScenesTransitionSetupDataSO::InitAndSetupScenes, void,
                StandardLevelScenesTransitionSetupDataSO* self,
                ::GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings, ::StringW backButtonText,
                bool startPaused) {
  auto customBeatmapLevel = il2cpp_utils::try_cast<SongCore::SongLoader::CustomBeatmapLevel>(self->get_beatmapLevel());
  if (!customBeatmapLevel) {
    StandardLevelScenesTransitionSetupDataSO_Init(self, playerSpecificSettings, backButtonText, startPaused);
    return;
  }

  // TODO: Fix environment override
  SceneTransitionHelper::Patch(customBeatmapLevel.value(), self->beatmapKey, self->targetEnvironmentInfo);

  StandardLevelScenesTransitionSetupDataSO_Init(self, playerSpecificSettings, backButtonText, startPaused);
}

void StandardLevelScenesTransitionSetupDataSOHook() {
  INSTALL_HOOK(ChromaLogger::Logger, StandardLevelScenesTransitionSetupDataSO_Init);
}

ChromaInstallHooks(StandardLevelScenesTransitionSetupDataSOHook)