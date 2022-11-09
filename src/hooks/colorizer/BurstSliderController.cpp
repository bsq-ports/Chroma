#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "colorizer/BombColorizer.hpp"
#include "colorizer/NoteColorizer.hpp"

#include "GlobalNamespace/ColorManager.hpp"
#include "GlobalNamespace/MultiplayerConnectedPlayerObstacleController.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"

#include "GlobalNamespace/BurstSliderGameNoteController.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/NoteJump.hpp"
#include "GlobalNamespace/NoteMovement.hpp"

#include "utils/ChromaUtils.hpp"

#include "AnimationHelper.hpp"
#include "ChromaObjectData.hpp"

#include "utils/ChromaAudioTimeSourceHelper.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace Chroma;
using namespace ChromaUtils;

MAKE_HOOK_MATCH(
    BurstSliderGameNoteController_Init, &BurstSliderGameNoteController::Init,
    void, BurstSliderGameNoteController *self,
    ::GlobalNamespace::NoteData *noteData, float worldRotation,
    ::UnityEngine::Vector3 moveStartPos, ::UnityEngine::Vector3 moveEndPos,
    ::UnityEngine::Vector3 jumpEndPos, float moveDuration, float jumpDuration,
    float jumpGravity,
    ::GlobalNamespace::NoteVisualModifierType noteVisualModifierType,
    float uniformScale) {
  BurstSliderGameNoteController_Init(self, noteData, worldRotation,
                                     moveStartPos, moveEndPos, jumpEndPos,
                                     moveDuration, jumpDuration, jumpGravity,
                                     noteVisualModifierType, uniformScale);

  if (!ChromaController::DoChromaHooks() ||
      !ChromaController::DoColorizerSabers()) {
    return;
  }

  auto chromaData =
      ChromaObjectDataManager::ChromaObjectDatas.find(self->noteData);
  if (chromaData != ChromaObjectDataManager::ChromaObjectDatas.end()) {
    auto const &color = chromaData->second.Color;

    if (color) {
      NoteColorizer::ColorizeNote(self, *color);
    }
  }
}
void BurstSliderControllerHook(Logger &logger) {
  INSTALL_HOOK(getLogger(), BurstSliderGameNoteController_Init);
}

ChromaInstallHooks(BurstSliderControllerHook)