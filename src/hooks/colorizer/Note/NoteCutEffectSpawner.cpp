#include "Chroma.hpp"
#include "ChromaLogger.hpp"
#include "ChromaController.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/NoteCutCoreEffectsSpawner.hpp"
#include "GlobalNamespace/NoteDebrisSpawner.hpp"
#include "GlobalNamespace/BombExplosionEffect.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/NoteCutHapticEffect.hpp"
#include "colorizer/NoteColorizer.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Random.hpp"
#include "UnityEngine/Space.hpp"
#include "UnityEngine/Transform.hpp"

#include "hooks/colorizer/Note/ColorManager.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Chroma;

// If this is true, we disable debris spawning in hooks
static bool global_DisableDebris_Disable = false;
static bool DisableDebrisOpt(NoteController* noteController) {
  if (!noteController) {
    ChromaLogger::Logger.debug("DisableDebrisOpt: noteController is null");
    return false;
  }
  if (!noteController->_noteData) {
    ChromaLogger::Logger.debug("DisableDebrisOpt: noteController->_noteData is null (noteController={} )", fmt::ptr(noteController));
    return false;
  }
  auto it = getObjectAD(noteController->_noteData);
  if (it) {
    auto const& chromaData = *it;
    std::optional<bool> disableDebris = chromaData.DisableDebris;
    return disableDebris.has_value() && disableDebris.value();
  }
  return false;
}

MAKE_HOOK_MATCH(NoteCutEffectSpawner_SpawnNoteCutEffect, &NoteCutCoreEffectsSpawner::SpawnNoteCutEffect, void,
                NoteCutCoreEffectsSpawner* self, ByRef<GlobalNamespace::NoteCutInfo> noteCutInfo,
                GlobalNamespace::NoteController* noteController, int sparkleParticlesCount, int explosionParticlesCount) {
  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    return NoteCutEffectSpawner_SpawnNoteCutEffect(self, noteCutInfo, noteController, sparkleParticlesCount, explosionParticlesCount);
  }

  global_DisableDebris_Disable = DisableDebrisOpt(noteController);

  ColorManagerColorForType::EnableColorOverride(noteController);
  NoteCutEffectSpawner_SpawnNoteCutEffect(self, noteCutInfo, noteController, sparkleParticlesCount, explosionParticlesCount);
  ColorManagerColorForType::DisableColorOverride();

  // reset state
  global_DisableDebris_Disable = false;
}

MAKE_HOOK_MATCH(NoteCutEffectSpawner_SpawnBombCutEffect, &NoteCutCoreEffectsSpawner::SpawnBombCutEffect, void,
                NoteCutCoreEffectsSpawner* self, ByRef<GlobalNamespace::NoteCutInfo> noteCutInfo,
                GlobalNamespace::NoteController* noteController) {
  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    return NoteCutEffectSpawner_SpawnBombCutEffect(self, noteCutInfo, noteController);
  }

  global_DisableDebris_Disable = DisableDebrisOpt(noteController);

  NoteCutEffectSpawner_SpawnBombCutEffect(self, noteCutInfo, noteController);

  // reset state
  global_DisableDebris_Disable = false;
}

MAKE_HOOK_MATCH(NoteDebrisSpawner_SpawnDebris, &NoteDebrisSpawner::SpawnDebris, void, NoteDebrisSpawner* self,
                NoteData_GameplayType noteGameplayType, Vector3 cutPoint, Vector3 cutNormal, float saberSpeed, Vector3 saberDir,
                Vector3 notePos, Quaternion noteRotation, Vector3 noteScale, ColorType colorType, float timeToNextColorNote,
                Vector3 moveVec) {

  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    return NoteDebrisSpawner_SpawnDebris(self, noteGameplayType, cutPoint, cutNormal, saberSpeed, saberDir, notePos, noteRotation,
                                         noteScale, colorType, timeToNextColorNote, moveVec);
  }

  // disable debris
  if (global_DisableDebris_Disable) return;

  return NoteDebrisSpawner_SpawnDebris(self, noteGameplayType, cutPoint, cutNormal, saberSpeed, saberDir, notePos, noteRotation, noteScale,
                                       colorType, timeToNextColorNote, moveVec);
}

MAKE_HOOK_MATCH(BombExplosionEffect_SpawnExplosion, &BombExplosionEffect::SpawnExplosion, void, BombExplosionEffect* self, Vector3 pos) {
  // Do nothing if Chroma shouldn't run or debris is disabled
  if (!ChromaController::DoChromaHooks()) {
    return BombExplosionEffect_SpawnExplosion(self, pos);
  }

  if (global_DisableDebris_Disable) return;

  return BombExplosionEffect_SpawnExplosion(self, pos);
}

// Fix for crash upon hitting bombs
// NoteCutCoreEffectsSpawner::SpawnBombCutEffect has an unused parameter that gets optimized out in assembly
// Calling the method manually will provide Hook_NoteCutEffectSpawner_SpawnBombCutEffect with the expected parameters
MAKE_HOOK_MATCH(NoteCutCoreEffectsSpawner_HandleNoteWasCut, &NoteCutCoreEffectsSpawner::HandleNoteWasCut, void,
                NoteCutCoreEffectsSpawner* self, NoteController* noteController, ByRef<NoteCutInfo> noteCutInfo) {
  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    return NoteCutCoreEffectsSpawner_HandleNoteWasCut(self, noteController, noteCutInfo);
  }

  if (noteController->_noteData->time + 0.5 >= self->_audioTimeSyncController->_songTime
      && noteController->_noteData->gameplayType == NoteData::GameplayType::Bomb) {
    self->SpawnBombCutEffect(noteCutInfo, noteController);
		self->_noteCutHapticEffect->HitNote(noteCutInfo->saberType, NoteCutHapticEffect::Type::Bomb);
    return;
  }

  return NoteCutCoreEffectsSpawner_HandleNoteWasCut(self, noteController, noteCutInfo);
}

void NoteCutEffectSpawnerHook() {
  INSTALL_HOOK(ChromaLogger::Logger, NoteCutEffectSpawner_SpawnNoteCutEffect);
  INSTALL_HOOK(ChromaLogger::Logger, NoteCutEffectSpawner_SpawnBombCutEffect);
  INSTALL_HOOK(ChromaLogger::Logger, NoteDebrisSpawner_SpawnDebris);
  INSTALL_HOOK(ChromaLogger::Logger, BombExplosionEffect_SpawnExplosion);
  INSTALL_HOOK(ChromaLogger::Logger, NoteCutCoreEffectsSpawner_HandleNoteWasCut);
}

ChromaInstallHooks(NoteCutEffectSpawnerHook)