#include "Chroma.hpp"

#include "ChromaController.hpp"

#include "GlobalNamespace/BaseNoteVisuals.hpp"
#include "GlobalNamespace/BombNoteController.hpp"
#include "GlobalNamespace/MirroredBombNoteController.hpp"
#include "GlobalNamespace/MultiplayerConnectedPlayerBombNoteController.hpp"
#include "GlobalNamespace/MultiplayerConnectedPlayerNoteController.hpp"
#include "GlobalNamespace/TutorialNoteController.hpp"
#include "GlobalNamespace/MirroredCubeNoteController.hpp"
#include "GlobalNamespace/GameNoteController.hpp"

#include "colorizer/BombColorizer.hpp"
#include "colorizer/NoteColorizer.hpp"
#include "utils/ChromaUtils.hpp"

using namespace GlobalNamespace;
using namespace Chroma;

bool IsBombType(NoteControllerBase* noteController)
{
    static std::unordered_set<Il2CppClass*> bombTypes = {
            classof(BombNoteController*),
            classof(MultiplayerConnectedPlayerBombNoteController*),
            classof(MirroredBombNoteController*),
    };

    return bombTypes.find(noteController->klass) != bombTypes.end();
}

MAKE_HOOK_OFFSETLESS(BaseNoteVisuals_Awake, void, BaseNoteVisuals* self) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        BaseNoteVisuals_Awake(self);
        return;
    }

    if (IsBombType(self->noteController)) {
        BombColorizer::New(self->noteController);
    } else {
        NoteColorizer::New(self->noteController);
    }

    BaseNoteVisuals_Awake(self);
}

MAKE_HOOK_OFFSETLESS(BaseNoteVisuals_OnDestroy, void, BaseNoteVisuals* self) {
    BaseNoteVisuals_OnDestroy(self);
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        return;
    }

    if (IsBombType(self->noteController)) {
        BombColorizer::Colorizers.erase(self->noteController);
    } else {
        NoteColorizer::Colorizers.erase(self->noteController);
    }
}

void BaseNoteVisualsHook(Logger& logger) {
    INSTALL_HOOK_OFFSETLESS(logger, BaseNoteVisuals_Awake, il2cpp_utils::FindMethodUnsafe("", "BaseNoteVisuals", "Awake", 0));
    INSTALL_HOOK_OFFSETLESS(logger, BaseNoteVisuals_OnDestroy, il2cpp_utils::FindMethodUnsafe("", "BaseNoteVisuals", "OnDestroy", 0));
}

ChromaInstallHooks(BaseNoteVisualsHook)