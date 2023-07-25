#pragma once

#include <string>

#include "custom-types/shared/delegate.hpp"

#include "UnityEngine/Color.hpp"

#include "GlobalNamespace/BasicBeatmapEventType.hpp"
#include "GlobalNamespace/ILightWithId.hpp"
#include "GlobalNamespace/LightWithIdManager.hpp"
#include "GlobalNamespace/BasicBeatmapEventData.hpp"
#include "System/Action_1.hpp"
#include "Tweening/ColorTween.hpp"

#include "ChromaController.hpp"
#include "lighting/ColorTween.hpp"
#include "LerpType.hpp"

#include "sombrero/shared/ColorUtils.hpp"
#include "sombrero/shared/HSBColor.hpp"
#include "tracks/shared/Animation/Easings.h"
#include "custom-json-data/shared/CJDLogger.h"
#include "main.hpp"

namespace Chroma::Tween {

constexpr auto ChromaTweenDiscriminator = 521912; // random number

struct ChromaColorTweenData {
  Functions easing = Functions::easeLinear;
  Chroma::LerpType lerpType = LerpType::RGB;
  GlobalNamespace::BasicBeatmapEventData* PreviousEvent = nullptr;

  int id;
  SafePtr<Tweening::ColorTween> tween;
  SafePtr<GlobalNamespace::ILightWithId> lightWithId;

  ChromaColorTweenData(int id, SafePtr<Tweening::ColorTween> const& tween,
                       SafePtr<GlobalNamespace::ILightWithId> const& lightWithId)
      : id(id), tween(tween), lightWithId(lightWithId) {}

  [[nodiscard]] Sombrero::FastColor GetColor(float time) const {
    auto const& fromValue = tween->fromValue;
    auto const& toValue = tween->toValue;

    time = Easings::Interpolate(time, easing);
    switch (lerpType) {
    case Chroma::LerpType::RGB:
      return Sombrero::FastColor::LerpUnclamped(fromValue, toValue, time);

    case Chroma::LerpType::HSV:
      float fromH, fromS, fromV;
      float toH, toS, toV;
      Sombrero::FastColor::RGBToHSV(fromValue, fromH, fromS, fromV);
      Sombrero::FastColor::RGBToHSV(toValue, toH, toS, toV);

      return Sombrero::FastColor::HSVToRGB(std::lerp(fromH, toH, time), std::lerp(fromS, toS, time),
                                           std::lerp(fromV, toV, time))
          .Alpha(std::lerp(fromValue.a, toValue.a, time));
    default: {
      CJDLogger::Logger.fmtThrowError("Lerp not valid for id {} using lerp {}", id, (int)lerpType);
      throw std::runtime_error("Lerp not valid");
    }
    }
  }
};

static void SetColor(GlobalNamespace::ILightWithId* lightWithId,
                     GlobalNamespace::LightWithIdManager* lightWithIdManager, UnityEngine::Color const& color) {
  if (!lightWithId->get_isRegistered()) {
    return;
  }
  lightWithIdManager->didChangeSomeColorsThisFrame = true;
  lightWithId->ColorWasSet(color);
}

static Tweening::ColorTween* makeTween(Sombrero::FastColor const& fromValue, Sombrero::FastColor const& toValue,
                                       GlobalNamespace::ILightWithId* lightWithId,
                                       GlobalNamespace::LightWithIdManager* lightWithIdManager) {
  auto tween = Tweening::ColorTween::New_ctor();

  std::function<void(UnityEngine::Color)> SetColorFn = [=](UnityEngine::Color const& color) constexpr {
    SetColor(lightWithId, lightWithIdManager, color);
  };
  auto SetColorAction = custom_types::MakeDelegate<System::Action_1<UnityEngine::Color>*>(SetColorFn);

  // it's okay to use random numbers for ease, they will default to easeLinear
  // thanks beatgames
  tween->Init(fromValue, toValue, SetColorAction, 0, ChromaTweenDiscriminator, 0);

  return tween;
}
} // namespace Chroma::Tween