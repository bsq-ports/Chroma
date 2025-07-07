#include "colorizer/ParticleColorizer.hpp"
#include "colorizer/LightColorizer.hpp"
#include "GlobalNamespace/ColorExtensions.hpp"

#include "GlobalNamespace/ParticleSystemEventEffect.hpp"
#include "GlobalNamespace/MultipliedColorSO.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Sombrero;

Chroma::ParticleColorizer::ParticleColorizer(GlobalNamespace::ParticleSystemEventEffect* particleSystemEventEffect,
                                             GlobalNamespace::BasicBeatmapEventType BasicBeatmapEventType)
    : _particleSystemEventEffect(particleSystemEventEffect) {
  _lightColor0 = il2cpp_utils::cast<MultipliedColorSO>(particleSystemEventEffect->_lightColor0.ptr());
  _lightColor1 = il2cpp_utils::cast<MultipliedColorSO>(particleSystemEventEffect->_lightColor1.ptr());
  _highlightColor0 = il2cpp_utils::cast<MultipliedColorSO>(particleSystemEventEffect->_highlightColor0.ptr());
  _highlightColor1 = il2cpp_utils::cast<MultipliedColorSO>(particleSystemEventEffect->_highlightColor1.ptr());

  // not sure when the light colorizer will be made...
  LightColorizer::CreateLightColorizerContract(particleSystemEventEffect->_colorEvent,
                                               [this](LightColorizer& colorizer) constexpr { this->AssignLightColorizer(&colorizer); });
}

std::shared_ptr<ParticleColorizer> ParticleColorizer::New(GlobalNamespace::ParticleSystemEventEffect* particleSystemEventEffect,
                                                          GlobalNamespace::BasicBeatmapEventType BasicBeatmapEventType) {
  std::shared_ptr<ParticleColorizer> particleColorizer(new ParticleColorizer(particleSystemEventEffect, BasicBeatmapEventType));

  GetOrCreateColorizerList(BasicBeatmapEventType).emplace(particleColorizer);
  return particleColorizer;
}

std::unordered_set<std::shared_ptr<ParticleColorizer>>&
ParticleColorizer::GetOrCreateColorizerList(GlobalNamespace::BasicBeatmapEventType eventType) {
  auto it = Colorizers.find(eventType.value__);

  if (it == Colorizers.end()) {
    Colorizers[eventType.value__] = std::unordered_set<std::shared_ptr<ParticleColorizer>>();
    return Colorizers[eventType.value__];
  }

  return it->second;
}

void ParticleColorizer::Refresh() {
  Sombrero::FastColor color;
  Sombrero::FastColor afterHighlightColor;

  switch (_previousValue.value__) {
  case 0:
    _particleSystemEventEffect->_particleColor = _particleSystemEventEffect->_offColor;
    _particleSystemEventEffect->RefreshParticles();
    break;

  case 1:
  case 5:
    color = GetNormalColor(_previousValue.value__, false);
    _particleSystemEventEffect->_particleColor = color;
    _particleSystemEventEffect->_offColor = color.Alpha(0.0f);
    _particleSystemEventEffect->RefreshParticles();
    break;

  case 2:
  case 6:
    color = GetHighlightColor(_previousValue.value__, false);
    _particleSystemEventEffect->_highlightColor = color;
    _particleSystemEventEffect->_offColor = color.Alpha(0.0f);
    afterHighlightColor = GetNormalColor(_previousValue.value__, false);
    _particleSystemEventEffect->_afterHighlightColor = afterHighlightColor;

    _particleSystemEventEffect->_particleColor =
        Sombrero::FastColor::Lerp(afterHighlightColor, color, _particleSystemEventEffect->_highlightValue);
    _particleSystemEventEffect->RefreshParticles();
    break;

  case 3:
  case 7:
  case -1:
    color = GetHighlightColor(_previousValue.value__, false);
    _particleSystemEventEffect->_highlightColor = color;
    _particleSystemEventEffect->_offColor = color.Alpha(0.0f);
    _particleSystemEventEffect->_particleColor = color;
    afterHighlightColor = _particleSystemEventEffect->_offColor;
    _particleSystemEventEffect->_afterHighlightColor = afterHighlightColor;

    _particleSystemEventEffect->_particleColor =
        Sombrero::FastColor::Lerp(afterHighlightColor, color, _particleSystemEventEffect->_highlightValue);
    _particleSystemEventEffect->RefreshParticles();
    break;
  }
}

void ParticleColorizer::Reset() {
  Colorizers.clear();
  Colorizers = {};
}
static bool IsColor0(int value) {
  return value == -1 || value == 0 || value == 1 || value == 2 || value == 3 || value == 4;
}

void ParticleColorizer::Callback(GlobalNamespace::BasicBeatmapEventData* beatmapEventData) {
  if (beatmapEventData == nullptr) {
    return;
  }

  _previousValue = beatmapEventData->value;
}

Sombrero::FastColor Chroma::ParticleColorizer::GetHighlightColor(int beatmapEventValue, bool colorBoost) const {
  auto* colorizer = this->lightColorizer.value();
  if (!IsColor0(beatmapEventValue)) {
    return colorizer->getColor()[1] * _highlightColor1->_multiplierColor;
  }

  return colorizer->getColor()[0] * _highlightColor0->_multiplierColor;
}
Sombrero::FastColor Chroma::ParticleColorizer::GetNormalColor(int beatmapEventValue, bool colorBoost) const {
  auto* colorizer = this->lightColorizer.value();
  if (!IsColor0(beatmapEventValue)) {
    return colorizer->getColor()[1] * _lightColor1->_multiplierColor;
  }

  return colorizer->getColor()[0] * _lightColor0->_multiplierColor;
}
void Chroma::ParticleColorizer::AssignLightColorizer(LightColorizer* colorizer) {
  lightColorizer = colorizer;
  colorizer->_lightSwitchEventEffect->BeatmapEventDidTrigger += { &ParticleColorizer::Callback, this };
  colorizer->_lightSwitchEventEffect->DidRefresh += { &ParticleColorizer::Refresh, this };
}
