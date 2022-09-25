#include "Chroma.hpp"

#include "ChromaController.hpp"
#include "utils/ChromaUtils.hpp"

#include "GlobalNamespace/ColorExtensions.hpp"
#include "hooks/colorizer/Note/ColorManager.hpp"
#include "GlobalNamespace/BeatEffectSpawner.hpp"

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


    // TODO: Transpile to force beat effect
    auto it = ChromaObjectDataManager::ChromaObjectDatas.find(noteController->noteData);

    if (it != ChromaObjectDataManager::ChromaObjectDatas.end()) {
        auto const& chromaData = it->second;
        std::optional<bool> disable = chromaData.DisableSpawnEffect;

        if (disable && disable.value()) {
            return;
        }
    }


    Sombrero::FastColor oldBombColorEffect = self->bombColorEffect;
    if (noteController->noteData->colorType == ColorType::None) {
        self->bombColorEffect = BombColorizer::GetBombColorizer(noteController)->getColor().Alpha(0.5f);
    } else {
        ColorManagerColorForType::EnableColorOverride(noteController);
    }
    BeatEffectSpawner_HandleNoteDidStartJump(self, noteController);
    if (noteController->noteData->colorType == ColorType::None) {
        self->bombColorEffect = oldBombColorEffect;
    }

    ColorManagerColorForType::DisableColorOverride();
}

void BeatEffectSpawnerHook(Logger& logger) {
    // TODO: DO TODO ABOVE
//    INSTALL_HOOK(logger, BeatEffectSpawner_HandleNoteDidStartJump);
}

ChromaInstallHooks(BeatEffectSpawnerHook)