#include "Chroma.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/SpriteRenderer.hpp"
#include "UnityEngine/Shader.hpp"
#include "UnityEngine/MaterialPropertyBlock.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Renderer.hpp"

#include "GlobalNamespace/MaterialPropertyBlockController.hpp"
#include "GlobalNamespace/NoteControllerBase.hpp"
#include "GlobalNamespace/NoteController.hpp"

#include <unordered_map>
#include "colorizer/BombColorizer.hpp"
#include "utils/ChromaUtils.hpp"

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Sombrero;
using namespace Chroma;

BombColorizer::ColorizerMap BombColorizer::Colorizers = BombColorizer::ColorizerMap();

int _simpleColor() {
  static int colorID = UnityEngine::Shader::PropertyToID("_SimpleColor");

  return colorID;
}

int _colorId() {
  static int colorID = UnityEngine::Shader::PropertyToID("_Color");

  return colorID;
}

void BombColorizer::Refresh() {
  Sombrero::FastColor const& color = getColor();

  // getter here causes the null value to instantiate
  // which then allows later use through field
  auto* bombMaterial = _materialPropertyBlockController->materialPropertyBlock;
  if (color == static_cast<Sombrero::FastColor>(bombMaterial->GetColor(_simpleColor()))) {
    return;
  }

  BombColorChanged.invoke(noteController, color);

  if (BombColorable) {
    return;
  }

  bombMaterial->SetColor(_simpleColor(), color);
  bombMaterial->SetColor(_colorId(), color);
  _materialPropertyBlockController->ApplyChanges();
}

BombColorizer::BombColorizer(GlobalNamespace::NoteControllerBase* noteController)
    : noteController(noteController),
      _materialPropertyBlockController(noteController->GetComponentInChildren<GlobalNamespace::MaterialPropertyBlockController*>()) {

  OriginalColor = noteController->GetComponentInChildren<Renderer*>()->get_material()->GetColor(_simpleColor());
  // getter here causes the null value to instantiate
  // which then allows later use through field
  auto* materialPropertyBlock = _materialPropertyBlockController->get_materialPropertyBlock();
  materialPropertyBlock->SetColor(_simpleColor(), OriginalColor);
  materialPropertyBlock->SetColor(_colorId(), OriginalColor);
}

std::optional<Sombrero::FastColor> BombColorizer::getGlobalColor() const {
  return GlobalColor;
}

std::optional<Sombrero::FastColor> BombColorizer::getGlobalColorStatic() {
  return GlobalColor;
}

void BombColorizer::GlobalColorize(std::optional<Sombrero::FastColor> const& color) {
  GlobalColor = color;
  for (auto& [_, colorizer] : Colorizers) {
    colorizer.Refresh();
  }
}

BombColorizer& BombColorizer::New(GlobalNamespace::NoteControllerBase* noteController) {
  return Colorizers.try_emplace(noteController, noteController).first->second;
}


void BombColorizer::Reset() {
  GlobalColor = std::nullopt;
  Colorizers.clear();
  BombColorChanged.clear();
}
