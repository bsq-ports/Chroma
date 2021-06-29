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



    if (noteController->noteData->colorType == ColorType::None) {
        self->bombColorEffect = ColorExtensions::ColorWithAlpha(BombColorizer::GetBombColorizer(noteController)->getColor(), 0.5f);
    } else {
        ColorManagerColorForType::EnableColorOverride(noteController);
    }
    BeatEffectSpawner_HandleNoteDidStartJump(self, noteController);

    ColorManagerColorForType::DisableColorOverride();
}

void BeatEffectSpawnerHook(Logger& logger) {
    INSTALL_HOOK_OFFSETLESS(logger, BeatEffectSpawner_HandleNoteDidStartJump, il2cpp_utils::FindMethodUnsafe("", "BeatEffectSpawner", "HandleNoteDidStartJump", 1));
}

ChromaInstallHooks(BeatEffectSpawnerHook)