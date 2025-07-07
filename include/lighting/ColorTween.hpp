#pragma once

#include <string>

#include "custom-types/shared/delegate.hpp"

#include "UnityEngine/Color.hpp"

#include "Tweening/ColorTween.hpp"
#include "GlobalNamespace/ILightWithId.hpp"
#include "GlobalNamespace/LightWithIdManager.hpp"

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
public:
  Functions easing = Functions::EaseLinear;
  Chroma::LerpType lerpType = LerpType::RGB;
  GlobalNamespace::BasicBeatmapEventData* PreviousEvent = nullptr;

  DECLARE_INSTANCE_FIELD(int, id);
  DECLARE_INSTANCE_FIELD(Sombrero::FastColor, fromValue);
  DECLARE_INSTANCE_FIELD(Sombrero::FastColor, toValue);
  DECLARE_INSTANCE_FIELD(GlobalNamespace::ILightWithId*, lightWithId);
  DECLARE_INSTANCE_FIELD(GlobalNamespace::LightWithIdManager *, lightWithIdManager);

  [[nodiscard]] Sombrero::FastColor GetColor(float time) const;

  static void SetColor(GlobalNamespace::ILightWithId * lightWithId, GlobalNamespace::LightWithIdManager * lightWithIdManager,
                       UnityEngine::Color const& color);

  DECLARE_CTOR(ctor, Sombrero::FastColor fromValue, Sombrero::FastColor toValue, GlobalNamespace::ILightWithId* lightWithId,
               GlobalNamespace::LightWithIdManager* lightWithIdManager, int id);

  DECLARE_SIMPLE_DTOR();

  // override GetValue
  DECLARE_OVERRIDE_METHOD_MATCH(UnityEngine::Color, GetValue, &Tweening::ColorTween::GetValue, float t);
};