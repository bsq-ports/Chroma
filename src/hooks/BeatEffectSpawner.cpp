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

#include "custom-json-data/shared/CustomBeatmapData.h"

#include "hooks/colorizer/Note/ColorManager.hpp"

using namespace Chroma;
using namespace GlobalNamespace;

inline bool BeatEffectForce(bool hideNoteSpawnEffect, NoteController* noteController) {
  auto it = ChromaObjectDataManager::ChromaObjectDatas.find(noteController->noteData);

  if (it != ChromaObjectDataManager::ChromaObjectDatas.end()) {
    auto const& chromaData = it->second;
    std::optional<bool> disable = chromaData.DisableSpawnEffect;

    if (disable) {
      return !disable.value();
    }
  }

  return hideNoteSpawnEffect;
}

MAKE_HOOK_MATCH(BeatEffectSpawner_HandleNoteDidStartJump, &BeatEffectSpawner::HandleNoteDidStartJump, void,
                BeatEffectSpawner* self, NoteController* noteController) {
  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    BeatEffectSpawner_HandleNoteDidStartJump(self, noteController);
    return;
  }

  /// TRANSPILE HERE
  //    if (self->initData->hideNoteSpawnEffect)
  if (BeatEffectForce(self->_initData->hideNoteSpawnEffect, noteController)) {
    return;
  }
  if (noteController->hidden) {
    return;
  }
  if (noteController->noteData->time + 0.1F < self->_audioTimeSyncController->songTime) {
    return;
  }
  ColorType colorType = noteController->noteData->colorType;
  Sombrero::FastColor a =
      (colorType != ColorType::None) ? self->_colorManager->ColorForType(colorType) : self->_bombColorEffect;
  auto beatEffect = self->_beatEffectPoolContainer->Spawn();
  beatEffect->didFinishEvent->Add(self->i___GlobalNamespace__IBeatEffectDidFinishEvent());
  beatEffect->get_transform()->SetPositionAndRotation(
      Sombrero::FastQuaternion(noteController->get_worldRotation()) * noteController->get_jumpStartPos() - Sombrero::FastVector3(0.F, 0.15F, 0.F),
      Sombrero::FastQuaternion::identity());
  beatEffect->Init(a * 1.F, self->_effectDuration, noteController->get_worldRotation());
}

void BeatEffectSpawnerHook() {
  // TODO: DO TODO ABOVE
  INSTALL_HOOK_ORIG(ChromaLogger::Logger, BeatEffectSpawner_HandleNoteDidStartJump);
}

ChromaInstallHooks(BeatEffectSpawnerHook)