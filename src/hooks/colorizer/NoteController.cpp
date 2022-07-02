#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "colorizer/BombColorizer.hpp"
#include "colorizer/NoteColorizer.hpp"

#include "GlobalNamespace/ColorManager.hpp"
#include "GlobalNamespace/MultiplayerConnectedPlayerObstacleController.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"

#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/BombNoteController.hpp"
#include "GlobalNamespace/NoteJump.hpp"
#include "GlobalNamespace/NoteMovement.hpp"

#include "utils/ChromaUtils.hpp"

#include "ChromaObjectData.hpp"
#include "AnimationHelper.hpp"

#include "utils/ChromaAudioTimeSourceHelper.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace Chroma;
using namespace ChromaUtils;

MAKE_HOOK_MATCH(
        NoteController_Update,
        &NoteController::ManualUpdate,
        void,
        NoteController * self
) {
    NoteController_Update(self);

    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks() || !ChromaController::DoColorizerSabers()) {
        return;
    }


    auto chromaData = ChromaObjectDataManager::ChromaObjectDatas.find(self->noteData);
    if (chromaData != ChromaObjectDataManager::ChromaObjectDatas.end()) {
        auto const& tracks = chromaData->second.Tracks;
        auto pathPointDefinition = chromaData->second.LocalPathColor;
        if (!tracks.empty() || pathPointDefinition) {
            NoteJump* noteJump = self->noteMovement->jump;
            float jumpDuration = noteJump->jumpDuration;
            float elapsedTime = ChromaTimeSourceHelper::getSongTimeChroma(noteJump->audioTimeSyncController) - (self->noteData->time - (jumpDuration * 0.5f));
            float normalTime = elapsedTime / jumpDuration;

            [[maybe_unused]] bool updated;
            std::optional<Sombrero::FastColor> colorOffset = AnimationHelper::GetColorOffset(pathPointDefinition,
                                                                                             tracks, normalTime,
                                                                                             updated, 0);

            if (colorOffset)
            {
                static auto BombNoteControllerKlass = classof(BombNoteController*);
                if (ASSIGNMENT_CHECK(BombNoteControllerKlass, self->klass)) {
                    BombColorizer::ColorizeBomb((BombNoteController*) self, colorOffset);
                } else {
                    NoteColorizer::ColorizeNote((NoteControllerBase*) self, colorOffset);
                }
            }
        }
    }
}

void NoteControllerHook(Logger &logger) {
    INSTALL_HOOK(getLogger(), NoteController_Update);
}

ChromaInstallHooks(NoteControllerHook)