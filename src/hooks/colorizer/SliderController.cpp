#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "colorizer/NoteColorizer.hpp"
#include "colorizer/SliderColorizer.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"

#include "GlobalNamespace/SliderMovement.hpp"
#include "GlobalNamespace/SliderData.hpp"
#include "GlobalNamespace/SliderController.hpp"

#include "utils/ChromaUtils.hpp"

#include "AnimationHelper.hpp"
#include "ChromaObjectData.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace Chroma;
using namespace ChromaUtils;

MAKE_HOOK_MATCH(SliderController_Update, &SliderController::ManualUpdate, void,
                SliderController *self) {
  SliderController_Update(self);

  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks() ||
      !ChromaController::DoColorizerSabers()) {
    return;
  }

  auto chromaData =
      ChromaObjectDataManager::ChromaObjectDatas.find(self->sliderData);
  if (chromaData != ChromaObjectDataManager::ChromaObjectDatas.end()) {
    auto const &tracks = chromaData->second.Tracks;
    auto pathPointDefinition = chromaData->second.LocalPathColor;
    if (!tracks.empty() || pathPointDefinition) {
      float jumpDuration = self->sliderMovement->jumpDuration;
      
      float duration = (jumpDuration * 0.75f) +
                       (self->sliderData->tailTime - self->sliderData->time);
      float normalTime =
          self->sliderMovement->timeSinceHeadNoteJump / (jumpDuration + duration);

      [[maybe_unused]] bool updated;
      std::optional<Sombrero::FastColor> colorOffset =
          AnimationHelper::GetColorOffset(pathPointDefinition, tracks,
                                          normalTime, updated, 0);

      if (colorOffset) {

        SliderColorizer::ColorizeSlider(self, colorOffset);
      }
    }
  }
}

void SliderControllerHook(Logger &logger) {
  INSTALL_HOOK(getLogger(), SliderController_Update);
}

ChromaInstallHooks(SliderControllerHook)