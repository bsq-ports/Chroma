#pragma once

#include <string>

#include "custom-types/shared/delegate.hpp"

#include "UnityEngine/Color.hpp"

#include "Tweening/ColorTween.hpp"
#include "GlobalNamespace/ILightWithId.hpp"
#include "GlobalNamespace/LightWithIdManager.hpp"
#include "GlobalNamespace/BasicBeatmapEventData.hpp"

#include "ChromaController.hpp"
#include "lighting/ColorTween.hpp"
#include "LerpType.hpp"

#include "sombrero/shared/ColorUtils.hpp"
#include "sombrero/shared/HSBColor.hpp"
#include "tracks/shared/Animation/Easings.h"
#include "custom-json-data/shared/CJDLogger.h"
#include "main.hpp"

namespace GlobalNamespace {
class BasicBeatmapEventData;
class LightWithIdManager;
} // namespace GlobalNamespace

namespace Tweening {
class ColorTween;
} // namespace Tweening

DECLARE_CLASS_CODEGEN(Chroma::Tween, ChromaTween, Tweening::ColorTween) {
private:
  DECLARE_INSTANCE_FIELD(GlobalNamespace::ILightWithId*, lightWithId);
  DECLARE_INSTANCE_FIELD(GlobalNamespace::LightWithIdManager*, lightWithIdManager);

public:
  DECLARE_INSTANCE_FIELD(int, id);
  DECLARE_INSTANCE_FIELD(GlobalNamespace::BasicBeatmapEventData*, PreviousEvent);
  Functions easing = Functions::EaseLinear;
  Chroma::LerpType lerpType = LerpType::RGB;


  [[nodiscard]] Sombrero::FastColor GetColor(float time) const;

  void SetColor(UnityEngine::Color const& color);

  DECLARE_CTOR(ctor, Sombrero::FastColor fromValue, Sombrero::FastColor toValue, GlobalNamespace::ILightWithId * lightWithId,
               GlobalNamespace::LightWithIdManager * lightWithIdManager, int id);

  DECLARE_SIMPLE_DTOR();

  // override GetValue
  DECLARE_OVERRIDE_METHOD_MATCH(UnityEngine::Color, GetValue, &Tweening::ColorTween::GetValue, float t);
};