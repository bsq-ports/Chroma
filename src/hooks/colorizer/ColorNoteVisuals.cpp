#include "Chroma.hpp"

#include "ChromaController.hpp"

#include "GlobalNamespace/MultiplayerConnectedPlayerNoteController.hpp"
#include "GlobalNamespace/TutorialNoteController.hpp"
#include "GlobalNamespace/GameNoteController.hpp"
#include "GlobalNamespace/ColorNoteVisuals.hpp"

#include "colorizer/NoteColorizer.hpp"
#include "utils/ChromaUtils.hpp"

using namespace GlobalNamespace;
using namespace Chroma;

MAKE_HOOK_MATCH(ColorNoteVisuals_HandleNoteControllerDidInit, &ColorNoteVisuals::HandleNoteControllerDidInit, void, ColorNoteVisuals* self,
                NoteControllerBase* noteController) {
  ColorNoteVisuals_HandleNoteControllerDidInit(self, noteController); // This calls the original method

  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    return;
  }

  auto NoteControllerCast = il2cpp_utils::try_cast<NoteController>(noteController);

  if (NoteControllerCast) {
    auto it = getObjectAD(NoteControllerCast.value()->_noteData);

    if (it) {
      auto const& chromaData = *it;

      std::optional<Sombrero::FastColor> const& color = chromaData.Color;

      NoteColorizer::ColorizeNote(noteController, color);
    }
  }
}

void ColorNoteVisualsHook() {
  INSTALL_HOOK(ChromaLogger::Logger, ColorNoteVisuals_HandleNoteControllerDidInit);
}

ChromaInstallHooks(ColorNoteVisualsHook)