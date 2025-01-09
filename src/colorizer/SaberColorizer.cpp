#include "Chroma.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/WaitForSecondsRealtime.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/MaterialPropertyBlock.hpp"
#include "GlobalNamespace/SaberModelController.hpp"
#include "GlobalNamespace/SetSaberGlowColor.hpp"
#include "GlobalNamespace/SetSaberFakeGlowColor.hpp"
#include "GlobalNamespace/TubeBloomPrePassLight.hpp"
#include "GlobalNamespace/SaberTrail.hpp"
#include "GlobalNamespace/Parametric3SliceSpriteController.hpp"
#include "GlobalNamespace/SaberManager.hpp"
#include "GlobalNamespace/SaberBurnMarkSparkles.hpp"
#include "GlobalNamespace/ColorManager.hpp"
#include "UnityEngine/LineRenderer.hpp"
#include "UnityEngine/ParticleSystem.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/Resources.hpp"
#include "hooks/SaberManager.hpp"

#include <unordered_map>
#include "utils/ChromaUtils.hpp"
#include "colorizer/SaberColorizer.hpp"

#include <cstdlib>

using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;
using namespace custom_types::Helpers;
using namespace Chroma;
using namespace Sombrero;

SaberColorizer::SaberColorizer(GlobalNamespace::Saber* saber, SaberModelController* saberModelController)
    : _saberModelController(saberModelController) {
  _saberType = saber->get_saberType();

  _saberTrail = _saberModelController->_saberTrail;
  _trailTintColor = _saberModelController->_saberTrail->_color;
  _saberLight = _saberModelController->_saberLight;

  _lastColor = _saberModelController->_colorManager->ColorForSaberType(_saberType);
  OriginalColor = _lastColor;
}

SaberColorizer& SaberColorizer::New(GlobalNamespace::Saber* saber) {
  auto* saberModelController = saber->get_gameObject()->GetComponentInChildren<SaberModelController*>(true);

  return Colorizers.try_emplace(saberModelController, saber, saberModelController).first->second;
}

std::optional<Sombrero::FastColor> SaberColorizer::GlobalColorGetter() {
  return GlobalColor[_saberType.value__];
}

void SaberColorizer::GlobalColorize(GlobalNamespace::SaberType saberType,
                                    std::optional<Sombrero::FastColor> const& color) {
  GlobalColor[saberType.value__] = color;
  for (auto const& c : GetColorizerList(saberType)) {
    c->Refresh();
  }
}

void SaberColorizer::Reset() {
  GlobalColor[0] = std::nullopt;
  GlobalColor[1] = std::nullopt;
  Colorizers.clear();

  SaberColorChanged.clear();
  ColorableModels.clear();
}

void SaberColorizer::Refresh() {
  Sombrero::FastColor const& color = getColor().Alpha(1.0F);
  if (color == Sombrero::FastColor(_lastColor)) {
    return;
  }

  static auto SetColor =
      FPtrWrapper<static_cast<void (UnityEngine::MaterialPropertyBlock::*)(StringW, UnityEngine::Color)>(
          &UnityEngine::MaterialPropertyBlock::SetColor)>::get();
  static auto SetPropertyBlock =
      FPtrWrapper<static_cast<void (UnityEngine::Renderer::*)(UnityEngine::MaterialPropertyBlock*)>(
          &UnityEngine::Renderer::SetPropertyBlock)>::get();
  static auto Refresh = FPtrWrapper<&Parametric3SliceSpriteController::Refresh>::get();

  _lastColor = color;
  if (!IsColorable(_saberModelController)) {
    auto _setSaberGlowColors = _saberModelController->_setSaberGlowColors;
    auto _setSaberFakeGlowColors = _saberModelController->_setSaberFakeGlowColors;

    auto* saberTrail = _saberTrail;
    saberTrail->_color = (color * _trailTintColor).Linear();

    if (_setSaberGlowColors) {
      for (auto setSaberGlowColor : _setSaberGlowColors) {
        if (!setSaberGlowColor) {
          continue;
        }

        MaterialPropertyBlock* materialPropertyBlock = setSaberGlowColor->_materialPropertyBlock;
        if (materialPropertyBlock == nullptr) {
          setSaberGlowColor->_materialPropertyBlock = MaterialPropertyBlock::New_ctor();
          materialPropertyBlock = setSaberGlowColor->_materialPropertyBlock;
        }

        auto propertyTintColorPairs = setSaberGlowColor->_propertyTintColorPairs;

        if (propertyTintColorPairs) {
          for (auto& propertyTintColorPair : propertyTintColorPairs) {
            if (propertyTintColorPair != nullptr) {
              SetColor(materialPropertyBlock, propertyTintColorPair->property,
                       color * Sombrero::FastColor(propertyTintColorPair->tintColor));
            }
          }
        }

        if (setSaberGlowColor->_meshRenderer != nullptr) {
          SetPropertyBlock(setSaberGlowColor->_meshRenderer, materialPropertyBlock);
        }
      }
    }

    if (_setSaberFakeGlowColors) {
      for (auto setSaberFakeGlowColor : _setSaberFakeGlowColors) {
        if (!setSaberFakeGlowColor) {
          continue;
        }

        auto parametric3SliceSprite = setSaberFakeGlowColor->_parametric3SliceSprite;
        parametric3SliceSprite->color = color * setSaberFakeGlowColor->_tintColor;
        Refresh(parametric3SliceSprite);
      }
    }

    if (_saberLight != nullptr) {
      _saberLight->color = color;
    }
  } else {
    ColorColorable(color);
  }

  SaberColorChanged.invoke(_saberType.value__, _saberModelController, color);
}

SaberColorizer& SaberColorizer::GetColorizer(GlobalNamespace::SaberModelController* saberModelController) {
  return Colorizers.at(saberModelController);
}

void SaberColorizer::RemoveColorizer(GlobalNamespace::SaberModelController* saberModelController) {
  Colorizers.erase(saberModelController);
}

std::unordered_set<SaberColorizer*> SaberColorizer::GetColorizerList(GlobalNamespace::SaberType saberType) {
  std::unordered_set<SaberColorizer*> colorizers;

  for (auto& [_, colorizer] : Colorizers) {
    if (colorizer._saberType.value__ == saberType.value__) {
      colorizers.emplace(&colorizer);
    }
  }

  return colorizers;
}

void SaberColorizer::ColorColorable(Sombrero::FastColor const& color) {
  // Nothing here I guess
}

void SaberColorizer::SetColorable(GlobalNamespace::SaberModelController* saberModelController, bool colorable) {
  if (colorable) {
    ColorableModels.emplace(saberModelController);
  } else {
    ColorableModels.erase(saberModelController);
  }
}

bool SaberColorizer::IsColorable(GlobalNamespace::SaberModelController* saberModelController) {
  return ColorableModels.contains(saberModelController);
}

SaberModelController* SaberColorizer::getSaberModelController() const {
  return _saberModelController;
}
