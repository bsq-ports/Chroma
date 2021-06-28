#include "Chroma.hpp"

#include "ChromaController.hpp"

#include "GlobalNamespace/BaseNoteVisuals.hpp"
#include "GlobalNamespace/BombNoteController.hpp"
#include "GlobalNamespace/MirroredBombNoteController.hpp"
#include "GlobalNamespace/ICubeNoteMirrorable.hpp"
#include "GlobalNamespace/MirroredCubeNoteController.hpp"
#include "GlobalNamespace/SaberBurnMarkArea.hpp"

#include "colorizer/NoteColorizer.hpp"
#include "colorizer/BombColorizer.hpp"
#include "utils/ChromaUtils.hpp"

using namespace GlobalNamespace;
using namespace Chroma;
using namespace UnityEngine;

void UpdateMirror(NoteControllerBase* noteController, GlobalNamespace::NoteControllerBase* followedNote)
{
    if (ASSIGNMENT_CHECK(classof(MirroredBombNoteController*), noteController->klass) )
    {
        BombColorizer::ColorizeBomb(noteController, BombColorizer::GetBombColorizer(followedNote)->getColor());
    }
    else
    {
        NoteColorizer::ColorizeNote(noteController, NoteColorizer::GetNoteColorizer(followedNote)->getColor());
    }
}

MAKE_HOOK_OFFSETLESS(MirroredNoteController_UpdatePositionAndRotationGeneric, void, MirroredNoteController_1<INoteMirrorable*>* self) {
    MirroredNoteController_UpdatePositionAndRotationGeneric(self);

    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        return;
    }

    UpdateMirror(self, il2cpp_utils::cast<NoteControllerBase>(self->followedNote));
}

MAKE_HOOK_OFFSETLESS(MirroredNoteController_UpdatePositionAndRotationCubeGeneric, void, MirroredNoteController_1<ICubeNoteMirrorable*>* self) {
    MirroredNoteController_UpdatePositionAndRotationCubeGeneric(self);

    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        return;
    }

    UpdateMirror(self, il2cpp_utils::cast<NoteControllerBase>(self->followedNote));
}


void Chroma::Hooks::MirroredNoteController() {
    auto iNoteGeneric = classof(GlobalNamespace::MirroredNoteController_1<INoteMirrorable*>*);
    auto iNoteCubeGeneric = classof(GlobalNamespace::MirroredNoteController_1<ICubeNoteMirrorable*>*);

    INSTALL_HOOK_OFFSETLESS(getLogger(), MirroredNoteController_UpdatePositionAndRotationGeneric, il2cpp_utils::FindMethodUnsafe(iNoteGeneric, "UpdatePositionAndRotation", 0));
    INSTALL_HOOK_OFFSETLESS(getLogger(), MirroredNoteController_UpdatePositionAndRotationCubeGeneric, il2cpp_utils::FindMethodUnsafe(iNoteCubeGeneric, "UpdatePositionAndRotation", 0));
}