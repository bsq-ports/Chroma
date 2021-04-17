#include "Chroma.hpp"

#include "ChromaController.hpp"
#include "utils/ChromaUtils.hpp"

#include "GlobalNamespace/BeatEffectSpawner.hpp"
#include "GlobalNamespace/BeatmapObjectSpawnController.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/MultiplayerConnectedPlayerNoteController.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"

#include "colorizer/NoteColorizer.hpp"
#include "ChromaObjectData.hpp"

using namespace Chroma;
using namespace GlobalNamespace;

MAKE_HOOK_OFFSETLESS(BeatEffectSpawner_HandleNoteDidStartJumpColorizer, void, BeatEffectSpawner* self, NoteController* noteController) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks() || ASSIGNMENT_CHECK(classof(MultiplayerConnectedPlayerNoteController*), noteController->klass)) {
        BeatEffectSpawner_HandleNoteDidStartJumpColorizer(self, noteController);
        return;
    }

    auto chromaData = std::static_pointer_cast<ChromaNoteData>(ChromaObjectDataManager::ChromaObjectDatas[noteController->noteData]);
    std::optional<bool> disable = chromaData->DisableSpawnEffect;

    if (disable && disable.value()) {
        return;
    }

    NoteColorizer::EnableNoteColorOverride(noteController);

    BeatEffectSpawner_HandleNoteDidStartJumpColorizer(self, noteController);

    NoteColorizer::DisableNoteColorOverride();
}


void Chroma::Hooks::BeatEffectSpawner() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), BeatEffectSpawner_HandleNoteDidStartJumpColorizer, il2cpp_utils::FindMethodUnsafe("", "BeatEffectSpawner", "HandleNoteDidStartJump", 1));
}