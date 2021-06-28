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
    ColorNoteVisuals_HandleNoteControllerDidInit(self, noteController); // This calls the original method

    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        return;
    }

    if (ASSIGNMENT_CHECK(classof(NoteController*), noteController->klass)) {
        auto it = ChromaObjectDataManager::ChromaObjectDatas.find(noteController->noteData);

        if (it != ChromaObjectDataManager::ChromaObjectDatas.end()) {
            auto chromaData = std::static_pointer_cast<ChromaNoteData>(it->second);

            std::optional<UnityEngine::Color> color = chromaData->Color;

            NoteColorizer::ColorizeNote(noteController, color);
        }
    }
}

void Chroma::Hooks::ColorNoteVisuals() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), ColorNoteVisuals_HandleNoteControllerDidInit, il2cpp_utils::FindMethodUnsafe("", "ColorNoteVisuals", "HandleNoteControllerDidInit", 1));
}