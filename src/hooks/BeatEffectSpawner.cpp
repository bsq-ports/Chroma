#include "Chroma.hpp"

#include "ChromaController.hpp"
#include "utils/ChromaUtils.hpp"

#include "GlobalNamespace/ColorExtensions.hpp"
#include "hooks/colorizer/Note/ColorManager.hpp"
#include "GlobalNamespace/BeatEffectSpawner.hpp"
#include "GlobalNamespace/BeatmapObjectSpawnController.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/MultiplayerConnectedPlayerNoteController.hpp"
#include "GlobalNamespace/TutorialNoteController.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"

#include "colorizer/BombColorizer.hpp"
#include "colorizer/NoteColorizer.hpp"
#include "ChromaObjectData.hpp"

using namespace Chroma;
using namespace GlobalNamespace;

MAKE_HOOK_MATCH(BeatEffectSpawner_HandleNoteDidStartJump, &BeatEffectSpawner::HandleNoteDidStartJump, void, BeatEffectSpawner* self, NoteController* noteController) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        BeatEffectSpawner_HandleNoteDidStartJump(self, noteController);
        return;
    }

    auto it = ChromaObjectDataManager::ChromaObjectDatas.find(noteController->noteData);

    if (it != ChromaObjectDataManager::ChromaObjectDatas.end()) {
        auto const& chromaData = it->second;
        std::optional<bool> disable = chromaData.DisableSpawnEffect;

        if (disable && disable.value()) {
            return;
        }
    }



    if (noteController->noteData->colorType == ColorType::None) {
        self->bombColorEffect = BombColorizer::GetBombColorizer(noteController)->getColor().Alpha(0.5f);
    } else {
        ColorManagerColorForType::EnableColorOverride(noteController);
    }
    BeatEffectSpawner_HandleNoteDidStartJump(self, noteController);

    ColorManagerColorForType::DisableColorOverride();
}

void BeatEffectSpawnerHook(Logger& logger) {
    INSTALL_HOOK(logger, BeatEffectSpawner_HandleNoteDidStartJump);
}

ChromaInstallHooks(BeatEffectSpawnerHook)