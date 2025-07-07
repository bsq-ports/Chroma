#include "lighting/ColorTween.hpp"

#include "GlobalNamespace/LightWithIdManager.hpp"
#include "GlobalNamespace/ILightWithId.hpp"

DEFINE_TYPE(Chroma::Tween, ChromaTween);

void Chroma::Tween::ChromaTween::ctor(Sombrero::FastColor fromValue, Sombrero::FastColor toValue,
                                      GlobalNamespace::ILightWithId* lightWithId, GlobalNamespace::LightWithIdManager* lightWithIdManager,
                                      int id) {
  // call super
  Tweening::ColorTween::_ctor();

  this->lightWithId = lightWithId;
  this->id = id;
  this->lightWithIdManager = lightWithIdManager;
  this->fromValue = fromValue;
  this->toValue = toValue;

  std::function<void(UnityEngine::Color)> SetColorFn = [this](UnityEngine::Color const& color) constexpr { SetColor(color); };
  auto* SetColorAction = custom_types::MakeDelegate<System::Action_1<UnityEngine::Color>*>(SetColorFn);
  Init(fromValue, toValue, SetColorAction, 0, GlobalNamespace::EaseType::Linear, 0);
}

void Chroma::Tween::ChromaTween::SetColor(UnityEngine::Color const& color) {
  if (!lightWithId->get_isRegistered()) {
    return;
  }
  lightWithIdManager->_didChangeSomeColorsThisFrame = true;
  lightWithId->ColorWasSet(color);
}

Sombrero::FastColor Chroma::Tween::ChromaTween::GetColor(float time) const {
  time = Easings::Interpolate(time, easing);
  switch (lerpType) {
  case Chroma::LerpType::RGB:
    return Sombrero::FastColor::LerpUnclamped(fromValue, toValue, time);

  case Chroma::LerpType::HSV:
    float fromH, fromS, fromV;
    float toH, toS, toV;
    Sombrero::FastColor::RGBToHSV(fromValue, fromH, fromS, fromV);
    Sombrero::FastColor::RGBToHSV(toValue, toH, toS, toV);

    return Sombrero::FastColor::HSVToRGB(std::lerp(fromH, toH, time), std::lerp(fromS, toS, time), std::lerp(fromV, toV, time))
        .Alpha(std::lerp(fromValue.a, toValue.a, time));
  default: {
    CJDLogger::Logger.fmtThrowError("Lerp not valid for id {} using lerp {}", id, (int)lerpType);
    throw std::runtime_error("Lerp not valid");
  }
  }
}

UnityEngine::Color Chroma::Tween::ChromaTween::GetValue(float time) {
  Sombrero::FastColor color = GetColor(time);
  return color;
}