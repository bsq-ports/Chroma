#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "colorizer/BombColorizer.hpp"
#include "colorizer/NoteColorizer.hpp"

#include "GlobalNamespace/ColorManager.hpp"
#include "GlobalNamespace/MultiplayerConnectedPlayerObstacleController.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"

#include "GlobalNamespace/BombNoteController.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/NoteJump.hpp"
#include "GlobalNamespace/NoteMovement.hpp"
#include "GlobalNamespace/VariableMovementDataProvider.hpp"

#include "utils/ChromaUtils.hpp"

#include "AnimationHelper.hpp"
#include "ChromaObjectData.hpp"

#include "utils/ChromaAudioTimeSourceHelper.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace Chroma;
using namespace ChromaUtils;

MAKE_HOOK_MATCH(NoteController_Init, &NoteController::Init, void, NoteController* self,
                ::GlobalNamespace::NoteData* noteData, ::ByRef<::GlobalNamespace::NoteSpawnData> noteSpawnData,
                float_t endRotation, float_t uniformScale, bool rotateTowardsPlayer, bool useRandomRotation) {
  NoteController_Init(self, noteData, noteSpawnData, endRotation, uniformScale, rotateTowardsPlayer,
                      useRandomRotation);

  if (!ChromaController::DoChromaHooks() || !ChromaController::DoColorizerSabers()) {
    return;
  }

  auto chromaData = ChromaObjectDataManager::ChromaObjectDatas.find(self->noteData);
  if (chromaData != ChromaObjectDataManager::ChromaObjectDatas.end()) {
    auto const& color = chromaData->second.Color;

    NoteColorizer::ColorizeNote(self, color);
  }
}

MAKE_HOOK_MATCH(NoteController_Update, &NoteController::ManualUpdate, void, NoteController* self) {
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
      NoteJump* noteJump = self->_noteMovement->_jump;
      float jumpDuration = noteJump->_variableMovementDataProvider->jumpDuration;
      float elapsedTime = ChromaTimeSourceHelper::getSongTimeChroma(noteJump->_audioTimeSyncController) -
                          (self->noteData->time - (jumpDuration * 0.5F));
      float normalTime = elapsedTime / jumpDuration;

      [[maybe_unused]] bool updated = false;
      std::optional<Sombrero::FastColor> colorOffset =
          AnimationHelper::GetColorOffset(pathPointDefinition, tracks, normalTime, updated, 0);

      if (colorOffset) {
        static auto* BombNoteControllerKlass = classof(BombNoteController*);
        if (ASSIGNMENT_CHECK(BombNoteControllerKlass, self->klass)) {
          BombColorizer::ColorizeBomb((BombNoteController*)self, colorOffset);
        } else {
          NoteColorizer::ColorizeNote((NoteControllerBase*)self, colorOffset);
        }
      }
    }
  }
}

void NoteControllerHook() {
  INSTALL_HOOK(ChromaLogger::Logger, NoteController_Init);
  INSTALL_HOOK(ChromaLogger::Logger, NoteController_Update);
}

ChromaInstallHooks(NoteControllerHook)