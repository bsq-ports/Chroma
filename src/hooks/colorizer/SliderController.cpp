#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "colorizer/NoteColorizer.hpp"
#include "colorizer/SliderColorizer.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"

#include "GlobalNamespace/SliderController.hpp"
#include "GlobalNamespace/SliderData.hpp"
#include "GlobalNamespace/SliderMovement.hpp"
#include "GlobalNamespace/VariableMovementDataProvider.hpp"

#include "utils/ChromaUtils.hpp"
#include "utils/VariableMovementHelper.hpp"

#include "AnimationHelper.hpp"
#include "ChromaObjectData.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace Chroma;
using namespace ChromaUtils;

MAKE_HOOK_MATCH(SliderController_Init, &SliderController::Init, void, SliderController* self,
                ::GlobalNamespace::SliderController_LengthType lengthType, ::GlobalNamespace::SliderData* sliderData,
                ::ByRef<::GlobalNamespace::SliderSpawnData> sliderSpawnData, float_t noteUniformScale,
                float_t randomValue) {
  SliderController_Init(self, lengthType, sliderData, sliderSpawnData, noteUniformScale, randomValue);

  if (!ChromaController::DoChromaHooks() || !ChromaController::DoColorizerSabers()) {
    return;
  }

  auto const& chromaData =  getObjectAD(self->_sliderData);
  if (chromaData) {
    auto const& color = chromaData->Color;

    if (color) {
      SliderColorizer::ColorizeSlider(self, color);
    }
  }
}

MAKE_HOOK_MATCH(SliderController_Update, &SliderController::ManualUpdate, void, SliderController* self) {
  SliderController_Update(self);

  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks() || !ChromaController::DoColorizerSabers()) {
    return;
  }

  auto const& chromaData =  getObjectAD(self->_sliderData);
  if (chromaData) {
    auto const& tracks = chromaData->Tracks;

    auto pathPointDefinition = chromaData->LocalPathColor;
    if (!tracks.empty() || pathPointDefinition) {
      VariableMovementW movement(self->_sliderMovement->_variableMovementDataProvider);
      float jumpDuration = movement.jumpDuration;

      float duration = (jumpDuration * 0.75F) + (self->_sliderData->tailTime - self->_sliderData->time);
      float normalTime = self->sliderMovement->timeSinceHeadNoteJump / (jumpDuration + duration);

      [[maybe_unused]] bool updated = false;
      std::optional<Sombrero::FastColor> colorOffset =
          AnimationHelper::GetColorOffset(pathPointDefinition, tracks, normalTime, updated, TimeUnit());

      if (colorOffset) {

        SliderColorizer::ColorizeSlider(self, colorOffset);
      }
    }
  }
}

void SliderControllerHook() {
  INSTALL_HOOK(ChromaLogger::Logger, SliderController_Init);
  INSTALL_HOOK(ChromaLogger::Logger, SliderController_Update);
}

ChromaInstallHooks(SliderControllerHook)
