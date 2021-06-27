#include "Chroma.hpp"

#include "ChromaController.hpp"

#include "GlobalNamespace/BaseNoteVisuals.hpp"
#include "GlobalNamespace/BombNoteController.hpp"
#include "GlobalNamespace/MirroredBombNoteController.hpp"
#include "GlobalNamespace/MirroredNoteController_1.hpp"
#include "GlobalNamespace/INoteMirrorable.hpp"
#include "GlobalNamespace/ICubeNoteMirrorable.hpp"
#include "GlobalNamespace/MultiplayerConnectedPlayerNoteController.hpp"
#include "GlobalNamespace/TutorialNoteController.hpp"
#include "GlobalNamespace/MirroredCubeNoteController.hpp"
#include "GlobalNamespace/SaberBurnMarkArea.hpp"

#include "UnityEngine/ParticleSystem.hpp"
#include "UnityEngine/ParticleSystem_MainModule.hpp"
#include "UnityEngine/ParticleSystem_MinMaxGradient.hpp"

#include "colorizer/NoteColorizer.hpp"
#include "colorizer/BombColorizer.hpp"
#include "utils/ChromaUtils.hpp"

using namespace GlobalNamespace;
using namespace Chroma;
using namespace UnityEngine;

std::vector<ParticleSystem*> _burnMarksPSNote;

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

MAKE_HOOK_OFFSETLESS(MirroredNoteController_UpdatePositionAndRotation, void, MirroredNoteController_1<Il2CppClass*>* self) {
    MirroredNoteController_UpdatePositionAndRotation(self);

    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        return;
    }

    UpdateMirror(self, il2cpp_utils::cast<NoteControllerBase>(self->followedNote));
}


void Chroma::Hooks::MirroredNoteController() {
    auto iNoteGeneric = classof(GlobalNamespace::MirroredNoteController_1<INoteMirrorable*>*);
    auto iNoteCubeGeneric = classof(GlobalNamespace::MirroredNoteController_1<ICubeNoteMirrorable*>*);

    INSTALL_HOOK_OFFSETLESS(getLogger(), MirroredNoteController_UpdatePositionAndRotation, il2cpp_utils::FindMethodUnsafe(iNoteGeneric, "UpdatePositionAndRotation", 0));
    INSTALL_HOOK_OFFSETLESS(getLogger(), MirroredNoteController_UpdatePositionAndRotation, il2cpp_utils::FindMethodUnsafe(iNoteCubeGeneric, "UpdatePositionAndRotation", 0));
}