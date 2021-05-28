#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/MirroredCubeNoteController.hpp"
#include "GlobalNamespace/SaberType.hpp"
#include "colorizer/NoteColorizer.hpp"
#include "UnityEngine/Random.hpp"
#include "UnityEngine/Space.hpp"
#include "UnityEngine/Transform.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Chroma;

MAKE_HOOK_OFFSETLESS(MirroredCubeNoteController_Mirror,void,MirroredCubeNoteController* self, GlobalNamespace::NoteController* noteController) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        MirroredCubeNoteController_Mirror(self, noteController);
        return;
    }
    NoteColorizer::EnableNoteColorOverride(noteController);
    MirroredCubeNoteController_Mirror(self, noteController);
    NoteColorizer::DisableNoteColorOverride();
}

void Chroma::Hooks::MirroredCubeNoteController() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), MirroredCubeNoteController_Mirror, il2cpp_utils::FindMethodUnsafe("", "MirroredCubeNoteController", "Mirror", 1));
}