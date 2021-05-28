#include "Chroma.hpp"

#include "ChromaController.hpp"

#include "GlobalNamespace/MultiplayerConnectedPlayerNoteController.hpp"
#include "GlobalNamespace/TutorialNoteController.hpp"
#include "GlobalNamespace/MirroredCubeNoteController.hpp"
#include "GlobalNamespace/GameNoteController.hpp"

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


    auto originalNoteController = noteController;


    auto mirrorNoteController = il2cpp_utils::try_cast<MirroredCubeNoteController>(noteController);
    if (mirrorNoteController) {
        auto tryCast = il2cpp_utils::try_cast<GameNoteController>(mirrorNoteController.value()->followedNote);
        if (tryCast) {
            noteController = tryCast.value();
        }
    }

    NoteColorizer::CNVStart(self, noteController);

    auto it = ChromaObjectDataManager::ChromaObjectDatas.find(noteController->noteData);

    if (it != ChromaObjectDataManager::ChromaObjectDatas.end()) {
        auto chromaData = std::static_pointer_cast<ChromaNoteData>(it->second);

        std::optional<UnityEngine::Color> color = chromaData->Color;

        if (color) {
            NoteColorizer::SetNoteColors(originalNoteController, color, color);
        } else {
            NoteColorizer::Reset(originalNoteController);
        }
    }

    NoteColorizer::EnableNoteColorOverride(noteController);

    ColorNoteVisuals_HandleNoteControllerDidInit(self, originalNoteController); // This calls the original method

    NoteColorizer::DisableNoteColorOverride();
}

void Chroma::Hooks::ColorNoteVisuals() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), ColorNoteVisuals_HandleNoteControllerDidInit, il2cpp_utils::FindMethodUnsafe("", "ColorNoteVisuals", "HandleNoteControllerDidInit", 1));
}