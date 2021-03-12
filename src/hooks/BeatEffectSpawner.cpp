#include "Chroma.hpp"

#include "ChromaController.hpp"

#include "GlobalNamespace/BeatEffectSpawner.hpp"
#include "GlobalNamespace/BeatmapObjectSpawnController.hpp"
#include "GlobalNamespace/NoteController.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"

#include "colorizer/NoteColorizer.hpp"

using namespace Chroma;
using namespace GlobalNamespace;

MAKE_HOOK_OFFSETLESS(BeatEffectSpawner_HandleNoteDidStartJumpColorizer, void, BeatEffectSpawner* self, NoteController* noteController) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        BeatEffectSpawner_HandleNoteDidStartJumpColorizer(self, noteController);
        return;
    }

    if (noteController->noteData && il2cpp_functions::class_is_assignable_from(noteController->noteData->klass, classof(CustomJSONData::CustomNoteData*))) {
        auto *pCustomNoteData = reinterpret_cast<CustomJSONData::CustomNoteData *>(noteController->noteData);

        if (pCustomNoteData->customData && pCustomNoteData->customData->value) {
//            getLogger().debug("I could disable notes");
            rapidjson::Value &dynData = *pCustomNoteData->customData->value;

            if (dynData.HasMember(DISABLESPAWNEFFECT)) {
                bool disable = dynData[DISABLESPAWNEFFECT].GetBool();

                if (disable) {
                    return;
                }
            }

        }

    }

    NoteColorizer::EnableNoteColorOverride(noteController);

    BeatEffectSpawner_HandleNoteDidStartJumpColorizer(self, noteController);

    NoteColorizer::DisableNoteColorOverride();
}


void Chroma::Hooks::BeatEffectSpawner() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), BeatEffectSpawner_HandleNoteDidStartJumpColorizer, il2cpp_utils::FindMethodUnsafe("", "BeatEffectSpawner", "HandleNoteDidStartJump", 1));
}