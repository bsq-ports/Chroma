#include "Chroma.hpp"

#include "ChromaLogger.hpp"
#include "ChromaController.hpp"
#include "ChromaObjectData.hpp"
#include "utils/ChromaUtils.hpp"

#include "colorizer/BombColorizer.hpp"
#include "colorizer/NoteColorizer.hpp"

#include "GlobalNamespace/BeatEffect.hpp"
#include "GlobalNamespace/ColorExtensions.hpp"
#include "GlobalNamespace/BeatEffectSpawner.hpp"
#include "GlobalNamespace/MemoryPoolContainer_1.hpp"
#include "GlobalNamespace/ILazyCopyHashSet_1.hpp"
#include "GlobalNamespace/LazyCopyHashSet_1.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/ColorManager.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"

#include "hooks/colorizer/Note/ColorManager.hpp"

using namespace Chroma;
using namespace GlobalNamespace;

static bool BeatEffectHide(bool original, NoteController* noteController) {
  auto it = ChromaObjectDataManager::ChromaObjectDatas.find(noteController->noteData);

  if (it != ChromaObjectDataManager::ChromaObjectDatas.end()) {
    auto const& chromaData = it->second;
    std::optional<bool> force = chromaData.SpawnEffect;

    if (force.has_value()) {
      // invert
      return !force.value();
    }
  }

  return original;
}

MAKE_HOOK_MATCH(BeatEffectSpawner_HandleNoteDidStartJump, &BeatEffectSpawner::HandleNoteDidStartJump, void, BeatEffectSpawner* self,
                NoteController* noteController) {
  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    BeatEffectSpawner_HandleNoteDidStartJump(self, noteController);
    return;
  }

  // no transpile needed yippe
  auto oldHideValue = self->_initData->hideNoteSpawnEffect;
  self->_initData->hideNoteSpawnEffect = BeatEffectHide(oldHideValue, noteController);

  BeatEffectSpawner_HandleNoteDidStartJump(self, noteController);

  self->_initData->hideNoteSpawnEffect = oldHideValue;
}

void BeatEffectSpawnerHook() {
  INSTALL_HOOK(ChromaLogger::Logger, BeatEffectSpawner_HandleNoteDidStartJump);
}

ChromaInstallHooks(BeatEffectSpawnerHook)