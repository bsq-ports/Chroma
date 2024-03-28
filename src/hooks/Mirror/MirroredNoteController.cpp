#include "Chroma.hpp"

#include "ChromaController.hpp"

#include "GlobalNamespace/BaseNoteVisuals.hpp"
#include "GlobalNamespace/BombNoteController.hpp"
#include "GlobalNamespace/MirroredBombNoteController.hpp"
#include "GlobalNamespace/IGameNoteMirrorable.hpp"
#include "GlobalNamespace/INoteMirrorable.hpp"
#include "GlobalNamespace/SaberBurnMarkArea.hpp"

#include "colorizer/NoteColorizer.hpp"
#include "colorizer/BombColorizer.hpp"
#include "utils/ChromaUtils.hpp"

using namespace GlobalNamespace;
using namespace Chroma;
using namespace UnityEngine;

template <>
struct ::il2cpp_utils::il2cpp_type_check::MetadataGetter<
    &GlobalNamespace::MirroredNoteController_1<IGameNoteMirrorable*>::UpdatePositionAndRotation> {
  static MethodInfo const* methodInfo() {
    return il2cpp_utils::FindMethod(classof(GlobalNamespace::MirroredNoteController_1<IGameNoteMirrorable*>*),
                                    "UpdatePositionAndRotation");
  }
};

template <>
struct ::il2cpp_utils::il2cpp_type_check::MetadataGetter<
    &GlobalNamespace::MirroredNoteController_1<INoteMirrorable*>::UpdatePositionAndRotation> {
  static MethodInfo const* methodInfo() {
    return il2cpp_utils::FindMethod(classof(GlobalNamespace::MirroredNoteController_1<INoteMirrorable*>*),
                                    "UpdatePositionAndRotation");
  }
};

void UpdateMirror(NoteControllerBase* noteController, GlobalNamespace::NoteControllerBase* followedNote) {
  static auto* MirroredBombNoteControllerKlass = classof(MirroredBombNoteController*);
  if (ASSIGNMENT_CHECK(MirroredBombNoteControllerKlass, noteController->klass)) {
    BombColorizer::ColorizeBomb(noteController, BombColorizer::GetBombColorizer(followedNote)->getColor());
  } else {
    if (ChromaController::DoColorizerSabers()) {
      NoteColorizer::ColorizeNote(noteController, NoteColorizer::GetNoteColorizer(followedNote)->getColor());
    }
  }
}

MAKE_HOOK_MATCH(MirroredNoteController_UpdatePositionAndRotationGeneric,
                &GlobalNamespace::MirroredNoteController_1<INoteMirrorable*>::UpdatePositionAndRotation, void,
                MirroredNoteController_1<INoteMirrorable*>* self) {
  MirroredNoteController_UpdatePositionAndRotationGeneric(self);

  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    return;
  }

  UpdateMirror(self, reinterpret_cast<NoteControllerBase*>(self->followedNote));
}

MAKE_HOOK_MATCH(MirroredNoteController_UpdatePositionAndRotationCubeGeneric,
                &GlobalNamespace::MirroredNoteController_1<IGameNoteMirrorable*>::UpdatePositionAndRotation, void,
                MirroredNoteController_1<IGameNoteMirrorable*>* self) {
  MirroredNoteController_UpdatePositionAndRotationCubeGeneric(self);

  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    return;
  }

  UpdateMirror(self, reinterpret_cast<NoteControllerBase*>(self->followedNote));
}

void MirroredNoteControllerHook() {
  auto* iNoteGeneric = classof(GlobalNamespace::MirroredNoteController_1<INoteMirrorable*>*);
  auto* iNoteCubeGeneric = classof(GlobalNamespace::MirroredNoteController_1<IGameNoteMirrorable*>*);

  INSTALL_HOOK(ChromaLogger::Logger, MirroredNoteController_UpdatePositionAndRotationGeneric);
  INSTALL_HOOK(ChromaLogger::Logger, MirroredNoteController_UpdatePositionAndRotationCubeGeneric);
}

ChromaInstallHooks(MirroredNoteControllerHook)