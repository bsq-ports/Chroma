#include "utils/ChromaUtils.hpp"
#include "colorizer/Monobehaviours/ChromaClashEffectController.hpp"

#include <cmath>
#include "colorizer/SaberColorizer.hpp"

#include "UnityEngine/ParticleSystem.hpp"

DEFINE_TYPE(Chroma, ChromaClashEffectController);

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Sombrero;

void ChromaClashEffectController::Init(UnityEngine::ParticleSystem* sparkleParticleSystem,
                                       UnityEngine::ParticleSystem* glowParticleSystem,
                                       GlobalNamespace::ColorManager* colorManager) {
  _sparkleParticleSystem = sparkleParticleSystem;
  _glowParticleSystem = glowParticleSystem;
  _colors[0] = colorManager->ColorForSaberType(SaberType::SaberA);
  _colors[1] = colorManager->ColorForSaberType(SaberType::SaberB);
  SaberColorizer::SaberColorChanged += { &ChromaClashEffectController::OnSaberColorChanged, this };
}

void ChromaClashEffectController::OnSaberColorChanged(int saberType,
                                                      GlobalNamespace::SaberModelController* /*saberModelController*/,
                                                      Sombrero::FastColor const& color) {
  float h = 0;
  float s = 0;
  float _ = 0;

  Sombrero::FastColor::RGBToHSV(color, h, s, _);
  Sombrero::FastColor effectColor = Sombrero::FastColor::HSVToRGB(h, s, 1);

  _colors[saberType] = effectColor;

  Sombrero::FastColor average = Sombrero::FastColor::Lerp(_colors[0], _colors[1], 0.5F);
  auto sparkleMain = _sparkleParticleSystem->get_main();
  sparkleMain.set_startColor(ParticleSystem::MinMaxGradient::op_Implicit___UnityEngine____ParticleSystem__MinMaxGradient(average));
  auto glowMain = _glowParticleSystem->get_main();
  glowMain.set_startColor(ParticleSystem::MinMaxGradient::op_Implicit___UnityEngine____ParticleSystem__MinMaxGradient(average));
}

void ChromaClashEffectController::OnDestroy() {
  SaberColorizer::SaberColorChanged -= { &ChromaClashEffectController::OnSaberColorChanged, this };
}
