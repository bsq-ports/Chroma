#include "Chroma.hpp"

#include "UnityEngine/MaterialPropertyBlock.hpp"
#include "UnityEngine/Shader.hpp"

#include "GlobalNamespace/MaterialPropertyBlockController.hpp"
#include "GlobalNamespace/ColorNoteVisuals.hpp"
#include "GlobalNamespace/NoteControllerBase.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/ColorManager.hpp"

#include <unordered_map>
#include "colorizer/NoteColorizer.hpp"
#include "colorizer/SliderColorizer.hpp"
#include "colorizer/SaberColorizer.hpp"
#include "ChromaController.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Sombrero;
using namespace Chroma;

static int _colorID() {
  static int colorID = UnityEngine::Shader::PropertyToID("_Color");

  return colorID;
}

NoteColorizer::NoteColorizer(GlobalNamespace::NoteControllerBase* noteController) : _noteController(noteController) {

  auto* colorNoteVisuals = _noteController->GetComponent<GlobalNamespace::ColorNoteVisuals*>();
  _colorNoteVisuals = colorNoteVisuals;
  CRASH_UNLESS(_noteController);
  CRASH_UNLESS(_colorNoteVisuals);

  _materialPropertyBlockControllers = colorNoteVisuals->_materialPropertyBlockControllers;
}

NoteColorizer* NoteColorizer::New(GlobalNamespace::NoteControllerBase* noteControllerBase) {
  if (!ChromaController::DoColorizerSabers()) {
    return nullptr;
  }

  return &Colorizers.try_emplace(noteControllerBase, noteControllerBase).first->second;
}

GlobalNamespace::ColorType NoteColorizer::getColorType() const {
  auto* noteData = _noteController->get_noteData();
  if (noteData != nullptr) {
    return noteData->colorType;
  }

  return ColorType::ColorA;
}

std::optional<Sombrero::FastColor> NoteColorizer::getGlobalColor() const {
  auto colorType = getColorType();

  if (colorType == ColorType::None) {
    return std::nullopt;
  }

  return GlobalColor[colorType.value__];
}

Sombrero::FastColor NoteColorizer::getOriginalColor() const {
  return _colorNoteVisuals->_colorManager->ColorForType(getColorType());
}

void NoteColorizer::GlobalColorize(std::optional<Sombrero::FastColor> const& color, GlobalNamespace::ColorType const& colorType) {
  GlobalColor[colorType.value__] = color;
  for (auto& [_, colorizer] : Colorizers) {
    colorizer.Refresh();
  }
  for (auto& [_, colorizer] : SliderColorizer::Colorizers) {
    colorizer.Refresh();
  }
}

void NoteColorizer::Reset() {
  GlobalColor[0] = std::nullopt;
  GlobalColor[1] = std::nullopt;
  Colorizers.clear();
  NoteColorChanged.clear();
}

void NoteColorizer::ColorizeSaber(GlobalNamespace::NoteController* noteController, NoteCutInfo const& noteCutInfo) {
  if (ChromaController::DoColorizerSabers()) {
    auto* noteData = noteController->noteData;
    SaberType saberType = noteCutInfo.saberType;
    if (noteData->colorType.value__ == saberType.value__) {
      SaberColorizer::ColorizeSaber(saberType, GetNoteColorizer(noteController)->getColor());
    }
  }
}

void NoteColorizer::Refresh() {
  if (!_colorNoteVisuals->get_isActiveAndEnabled()) {
    return;
  }

  Sombrero::FastColor const& color = getColor();
  if (color == Sombrero::FastColor(_colorNoteVisuals->_noteColor)) {
    return;
  }

  NoteColorChanged.invoke(_noteController, color, getColorType());
  if (NoteColorable) {
    return;
  }

  static auto ApplyChanges = FPtrWrapper<&GlobalNamespace::MaterialPropertyBlockController::ApplyChanges>::get();
  static auto SetColor = FPtrWrapper<static_cast<void (UnityEngine::MaterialPropertyBlock::*)(int, UnityEngine::Color)>(
      &UnityEngine::MaterialPropertyBlock::SetColor)>::get();

  _colorNoteVisuals->_noteColor = color;
  for (auto materialPropertyBlockController : _materialPropertyBlockControllers) {
    if (materialPropertyBlockController == nullptr) {
      continue;
    }

    if (materialPropertyBlockController->materialPropertyBlock == nullptr) {
      continue;
    }

    auto* propertyBlock = materialPropertyBlockController->materialPropertyBlock;
    auto originalColor = propertyBlock->GetColor(_colorID());

    SetColor(propertyBlock, _colorID(), color.Alpha(originalColor.a));
    ApplyChanges(materialPropertyBlockController);
  }
}
