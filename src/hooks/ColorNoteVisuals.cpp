#include "Chroma.hpp"

#include "ChromaController.hpp"

#include "GlobalNamespace/MultiplayerConnectedPlayerNoteController.hpp"
#include "GlobalNamespace/TutorialNoteController.hpp"

#include "colorizer/NoteColorizer.hpp"
#include "utils/ChromaUtils.hpp"

using namespace GlobalNamespace;
using namespace Chroma;

MAKE_HOOK_OFFSETLESS(ColorNoteVisuals_HandleNoteControllerDidInit, void, ColorNoteVisuals* self, NoteController* noteController) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        ColorNoteVisuals_HandleNoteControllerDidInit(self, noteController);
        return;
    }

    if (!ASSIGNMENT_CHECK(classof(MultiplayerConnectedPlayerNoteController*), noteController->klass) && !ASSIGNMENT_CHECK(classof(TutorialNoteController*), noteController->klass)) {
        NoteColorizer::CNVStart(self, noteController);

//        auto chromaData = std::static_pointer_cast<ChromaNoteData>(ChromaObjectDataManager::ChromaObjectDatas[noteController->noteData]);

//        auto color = chromaData->Color;

//        if (color) {
//
//            NoteColorizer::SetNoteColors(noteController, color, color);
//        } else {
//            NoteColorizer::Reset(noteController);
//        }

        NoteColorizer::EnableNoteColorOverride(noteController);
    }

    ColorNoteVisuals_HandleNoteControllerDidInit(self, noteController); // This calls the original method

    NoteColorizer::DisableNoteColorOverride();
}

void Chroma::Hooks::ColorNoteVisuals() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), ColorNoteVisuals_HandleNoteControllerDidInit, il2cpp_utils::FindMethodUnsafe("", "ColorNoteVisuals", "HandleNoteControllerDidInit", 1));
}