#include "Chroma.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/MultiplayerLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/BeatmapLevelSO.hpp"

#include "hooks/SceneTransition/SceneTransitionHelper.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Chroma;

MAKE_HOOK_MATCH(MultiplayerLevelScenesTransitionSetupDataSO_Init, &MultiplayerLevelScenesTransitionSetupDataSO::InitAndSetupScenes,
                void, MultiplayerLevelScenesTransitionSetupDataSO* self) {

  
  auto customBeatmapLevel = il2cpp_utils::try_cast<SongCore::SongLoader::CustomBeatmapLevel>(self->get_beatmapLevel());
  if (!customBeatmapLevel) {
    MultiplayerLevelScenesTransitionSetupDataSO_Init(self);
    return;
  }

  // TODO: Fix environment override
  SceneTransitionHelper::Patch(customBeatmapLevel.value(), self->beatmapKey, self->GetOrLoadMultiplayerEnvironmentInfo());

  MultiplayerLevelScenesTransitionSetupDataSO_Init(self);
}

void MultiplayerLevelScenesTransitionSetupDataSOHook() {
  INSTALL_HOOK(ChromaLogger::Logger, MultiplayerLevelScenesTransitionSetupDataSO_Init);
}

ChromaInstallHooks(MultiplayerLevelScenesTransitionSetupDataSOHook)