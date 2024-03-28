#include "Chroma.hpp"

#include "ChromaController.hpp"

#include "GlobalNamespace/BaseNoteVisuals.hpp"
#include "GlobalNamespace/BombNoteController.hpp"
#include "GlobalNamespace/MirroredBombNoteController.hpp"
#include "GlobalNamespace/MultiplayerConnectedPlayerBombNoteController.hpp"
#include "GlobalNamespace/GameNoteController.hpp"

#include "colorizer/BombColorizer.hpp"
#include "colorizer/NoteColorizer.hpp"
#include "utils/ChromaUtils.hpp"

using namespace GlobalNamespace;
using namespace Chroma;

bool IsBombType(NoteControllerBase* noteController) {
  static std::unordered_set<Il2CppClass*> bombTypes = {
    classof(BombNoteController*),
    classof(MultiplayerConnectedPlayerBombNoteController*),
    classof(MirroredBombNoteController*),
  };

  return bombTypes.contains(noteController->klass);
}

MAKE_HOOK_MATCH(BaseNoteVisuals_Awake, &BaseNoteVisuals::Awake, void, BaseNoteVisuals* self) {
  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    BaseNoteVisuals_Awake(self);
    return;
  }

  if (IsBombType(self->_noteController)) {
    BombColorizer::New(self->_noteController);
  } else {
    NoteColorizer::New(self->_noteController);
  }

  BaseNoteVisuals_Awake(self);
}

MAKE_HOOK_MATCH(BaseNoteVisuals_OnDestroy, &BaseNoteVisuals::OnDestroy, void, BaseNoteVisuals* self) {
  BaseNoteVisuals_OnDestroy(self);
  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    return;
  }

  if (IsBombType(self->_noteController)) {
    BombColorizer::Colorizers.erase(self->_noteController);
  } else {
    NoteColorizer::Colorizers.erase(self->_noteController);
  }
}

void BaseNoteVisualsHook() {
  INSTALL_HOOK(ChromaLogger::Logger, BaseNoteVisuals_Awake);
  INSTALL_HOOK(ChromaLogger::Logger, BaseNoteVisuals_OnDestroy);
}

ChromaInstallHooks(BaseNoteVisualsHook)