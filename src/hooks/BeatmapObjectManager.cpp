#include "Chroma.hpp"

#include "ChromaController.hpp"
#include "colorizer/NoteColorizer.hpp"

#include "GlobalNamespace/BeatmapObjectManager.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;
using namespace custom_types::Helpers;
using namespace GlobalNamespace;

MAKE_HOOK_OFFSETLESS(BeatmapObjectManager_NoteCutEvent, void, BeatmapObjectManager *self, NoteController *noteController,
                     NoteCutInfo& noteCutInfo) {
    if (!ChromaController::DoChromaHooks()) {
        BeatmapObjectManager_NoteCutEvent(self, noteController, noteCutInfo);
        return;
    }
    NoteColorizer::ColorizeSaber(noteController, noteCutInfo);
    BeatmapObjectManager_NoteCutEvent(self, noteController, noteCutInfo);
}

void Chroma::Hooks::BeatmapObjectManager() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), BeatmapObjectManager_NoteCutEvent,
                            il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectManager", "HandleNoteControllerNoteWasCut", 2));
}