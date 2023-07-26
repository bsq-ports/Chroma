#include "Chroma.hpp"

#include <unordered_map>
#include "colorizer/SliderColorizer.hpp"
#include "colorizer/NoteColorizer.hpp"
#include "ChromaController.hpp"
#include "utils/ChromaUtils.hpp"
#include "ChromaObjectData.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Sombrero;
using namespace Chroma;

SliderColorizer::SliderColorizer(GlobalNamespace::SliderController* sliderController)
    : _sliderController(sliderController) {
  CRASH_UNLESS(_sliderController);
}

SliderColorizer* SliderColorizer::New(GlobalNamespace::SliderController* sliderControllerBase) {
  if (!ChromaController::DoColorizerSabers()) {
    return nullptr;
  }

  return &Colorizers.try_emplace(sliderControllerBase, sliderControllerBase).first->second;
}

GlobalNamespace::ColorType SliderColorizer::getColorType() {
  if ((_sliderController != nullptr) && (_sliderController->sliderData != nullptr)) {
    auto* sliderData = _sliderController->sliderData;

    return sliderData->colorType;
  }

  return ColorType::ColorA;
}

std::optional<Sombrero::FastColor> SliderColorizer::GlobalColorGetter() {
  return NoteColorizer::GlobalColor[(int)getColorType()];
}

std::optional<Sombrero::FastColor> SliderColorizer::OriginalColorGetter() {
  return _sliderController->colorManager->ColorForType(getColorType());
}

void SliderColorizer::Reset() {
  Colorizers.clear();
  SliderColorChanged.clear();
}

void SliderColorizer::Refresh() {
  _sliderController->initColor = getColor();
}
