#include "Chroma.hpp"

#include <unordered_map>

#include "colorizer/SliderColorizer.hpp"
#include "colorizer/NoteColorizer.hpp"

#include "ChromaController.hpp"
#include "utils/ChromaUtils.hpp"
#include "ChromaObjectData.hpp"

#include "GlobalNamespace/SliderController.hpp"
#include "GlobalNamespace/ColorManager.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Sombrero;
using namespace Chroma;

SliderColorizer::SliderColorizer(GlobalNamespace::SliderController* sliderController) : _sliderController(sliderController) {
  CRASH_UNLESS(_sliderController);
}

SliderColorizer* SliderColorizer::New(GlobalNamespace::SliderController* sliderControllerBase) {
  if (!ChromaController::DoColorizerSabers()) {
    return nullptr;
  }

  return &Colorizers.try_emplace(sliderControllerBase, sliderControllerBase).first->second;
}

GlobalNamespace::ColorType SliderColorizer::getColorType() const {
  if ((_sliderController != nullptr) && (_sliderController->_sliderData != nullptr)) {
    auto* sliderData = _sliderController->_sliderData;

    return sliderData->colorType;
  }

  return ColorType::ColorA;
}

std::optional<Sombrero::FastColor> SliderColorizer::getGlobalColor() const {
  return NoteColorizer::GlobalColor[getColorType().value__];
}

Sombrero::FastColor SliderColorizer::getOriginalColor() const {
  return _sliderController->_colorManager->ColorForType(getColorType());
}

void SliderColorizer::Reset() {
  Colorizers.clear();
  SliderColorChanged.clear();
}

void SliderColorizer::Refresh() {
  _sliderController->_initColor = getColor();
}
