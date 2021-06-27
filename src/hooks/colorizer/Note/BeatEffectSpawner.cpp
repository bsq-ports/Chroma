#include "Chroma.hpp"

#include "ChromaController.hpp"
#include "utils/ChromaUtils.hpp"

#include "GlobalNamespace/BeatEffectSpawner.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/ColorExtensions.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"

#include "colorizer/NoteColorizer.hpp"
#include "colorizer/BombColorizer.hpp"
#include "hooks/colorizer/Note/ColorManager.hpp"

using namespace Chroma;
using namespace GlobalNamespace;

MAKE_HOOK_OFFSETLESS(colorizer_BeatEffectSpawner_HandleNoteDidStartJump, void, BeatEffectSpawner* self, NoteController* noteController) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        colorizer_BeatEffectSpawner_HandleNoteDidStartJump(self, noteController);
        return;
    }

    if (noteController->noteData->colorType == ColorType::None) {
        self->bombColorEffect = ColorExtensions::ColorWithAlpha(BombColorizer::GetBombColorizer(noteController)->getColor(), 0.5f);
    } else {
        ColorManagerColorForType::EnableColorOverride(noteController);
    }

    colorizer_BeatEffectSpawner_HandleNoteDidStartJump(self, noteController);

    ColorManagerColorForType::DisableColorOverride();
}


void Chroma::Hooks::colorizer_BeatEffectSpawner() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), colorizer_BeatEffectSpawner_HandleNoteDidStartJump, il2cpp_utils::FindMethodUnsafe("", "BeatEffectSpawner", "HandleNoteDidStartJump", 1));
}