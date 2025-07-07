#pragma once
#include <vector>
#include <string>
#include <optional>
#include <unordered_map>

#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

#include "UnityEngine/Color.hpp"

#include "GlobalNamespace/LightSwitchEventEffect.hpp"

#include "ChromaController.hpp"
#include "ColorTween.hpp"

#include "sombrero/shared/ColorUtils.hpp"
#include "tracks/shared/Animation/Easings.h"
#include "main.hpp"

namespace Chroma {
class LightColorizer;
}

namespace GlobalNamespace {
class BasicBeatmapEventData;
class ColorSO;
class ColorBoostBeatmapEventData;
class ILightWithId;
class LightWithIdManager;
} // namespace GlobalNamespace


DECLARE_CLASS_CODEGEN(Chroma, ChromaLightSwitchEventEffect, GlobalNamespace::LightSwitchEventEffect) {
public:
  //          std::array<Sombrero::FastColor, 4> OriginalColors;

  Sombrero::FastColor _lightColor0Mult;
  Sombrero::FastColor _lightColor1Mult;
  Sombrero::FastColor _highlightColor0Mult;
  Sombrero::FastColor _highlightColor1Mult;
  Sombrero::FastColor _lightColor0BoostMult;
  Sombrero::FastColor _lightColor1BoostMult;
  Sombrero::FastColor _highlightColor0BoostMult;
  Sombrero::FastColor _highlightColor1BoostMult;

  std::unordered_map<GlobalNamespace::ILightWithId*, SafePtr<Tween::ChromaTween>> ColorTweens;

  Chroma::LightColorizer* lightColorizer;

  DECLARE_INSTANCE_METHOD(void, Awake);
  DECLARE_INSTANCE_METHOD(void, OnDestroy);
  DECLARE_SIMPLE_DTOR();
  DECLARE_DEFAULT_CTOR();

public:
  static std::unordered_set<ChromaLightSwitchEventEffect*> livingLightSwitch;

  UnorderedEventCallback<GlobalNamespace::BasicBeatmapEventData*> BeatmapEventDidTrigger;
  UnorderedEventCallback<> DidRefresh;

  // improve speed, avoid codegen
  Sombrero::FastColor GetNormalColor(int beatmapEventValue, bool colorBoost);
  Sombrero::FastColor GetHighlightColor(int beatmapEventValue, bool colorBoost);
  static constexpr bool IsFixedDurationLightSwitch(int beatmapEventValue);
  static constexpr bool IsColor0(int beatmapEventValue);

  void HandleEvent(GlobalNamespace::BasicBeatmapEventData * BasicBeatmapEventData);
  void HandleBoostEvent(GlobalNamespace::ColorBoostBeatmapEventData * BasicBeatmapEventData);

  void Refresh(bool hard, std::optional<std::vector<GlobalNamespace::ILightWithId*>> const& selectLights,
               std::optional<GlobalNamespace::BasicBeatmapEventData*> BasicBeatmapEventData = std::nullopt,
               std::optional<Functions> easing = std::nullopt, std::optional<LerpType> lerpType = std::nullopt);

  void CopyValues(LightSwitchEventEffect * lightSwitchEventEffect);


  void RegisterLight(GlobalNamespace::ILightWithId * lightWithId, int key);
  void UnregisterLight(GlobalNamespace::ILightWithId * lightWithId);
};