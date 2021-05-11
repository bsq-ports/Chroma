#include "Chroma.hpp"

#include "ChromaController.hpp"
#include "utils/ChromaUtils.hpp"

#include "GlobalNamespace/BeatEffectSpawner.hpp"
#include "GlobalNamespace/BeatmapObjectSpawnController.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/MultiplayerConnectedPlayerNoteController.hpp"
#include "GlobalNamespace/TutorialNoteController.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"

#include "colorizer/NoteColorizer.hpp"
#include "ChromaObjectData.hpp"

using namespace Chroma;
using namespace GlobalNamespace;

MAKE_HOOK_OFFSETLESS(BeatEffectSpawner_HandleNoteDidStartJump, void, BeatEffectSpawner* self, NoteController* noteController) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        BeatEffectSpawner_HandleNoteDidStartJump(self, noteController);
        return;
    }

    auto it = ChromaObjectDataManager::ChromaObjectDatas.find(noteController->noteData);

    if (it != ChromaObjectDataManager::ChromaObjectDatas.end()) {
        auto chromaData = std::static_pointer_cast<ChromaNoteData>(it->second);
        std::optional<bool> disable = chromaData->DisableSpawnEffect;

        if (disable && disable.value()) {
            return;
        }
    }

    NoteColorizer::EnableNoteColorOverride(noteController);

    BeatEffectSpawner_HandleNoteDidStartJump(self, noteController);

    NoteColorizer::DisableNoteColorOverride();
}


void Chroma::Hooks::BeatEffectSpawner() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), BeatEffectSpawner_HandleNoteDidStartJump, il2cpp_utils::FindMethodUnsafe("", "BeatEffectSpawner", "HandleNoteDidStartJump", 1));
}