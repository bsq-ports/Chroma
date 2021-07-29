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

MAKE_HOOK_MATCH(BeatmapObjectManager_NoteCutEvent, &BeatmapObjectManager::HandleNoteControllerNoteWasCut, void, BeatmapObjectManager *self, NoteController *noteController,
                     ByRef<NoteCutInfo> noteCutInfo) {
    if (!ChromaController::DoChromaHooks()) {
        BeatmapObjectManager_NoteCutEvent(self, noteController, noteCutInfo);
        return;
    }
    NoteColorizer::ColorizeSaber(noteController, noteCutInfo.heldRef);
    BeatmapObjectManager_NoteCutEvent(self, noteController, noteCutInfo);
}

void BeatmapObjectManagerHook(Logger& logger) {
    INSTALL_HOOK(logger, BeatmapObjectManager_NoteCutEvent);
}

ChromaInstallHooks(BeatmapObjectManagerHook)