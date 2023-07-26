#include "lighting/ChromaLightSwitchEventEffect.hpp"
#include "lighting/ChromaEventData.hpp"
#include "lighting/LegacyLightHelper.hpp"
#include "lighting/ChromaGradientController.hpp"

#include "GlobalNamespace/EnvironmentColorType.hpp"
#include "GlobalNamespace/ColorManager.hpp"

#include "Tweening/SongTimeTweeningManager.hpp"
#include "lighting/LightIDTableManager.hpp"
#include "colorizer/LightColorizer.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;

DEFINE_TYPE(Chroma, ChromaLightSwitchEventEffect);

std::unordered_set<ChromaLightSwitchEventEffect*> ChromaLightSwitchEventEffect::livingLightSwitch;
std::unordered_map<Tweening::ColorTween*, Tween::ChromaColorTweenData*>
    ChromaLightSwitchEventEffect::ColorTweensMapping;

constexpr static GlobalNamespace::EnvironmentColorType GetLightColorTypeFromEventDataValue(int beatmapEventValue) {
  if (beatmapEventValue == 1 || beatmapEventValue == 2 || beatmapEventValue == 3 || beatmapEventValue == 4 ||
      beatmapEventValue == 0 || beatmapEventValue == -1) {
    return EnvironmentColorType::Color0;
  }
  if (beatmapEventValue == 5 || beatmapEventValue == 6 || beatmapEventValue == 7 || beatmapEventValue == 8) {
    return EnvironmentColorType::Color1;
  }
  if (beatmapEventValue == 9 || beatmapEventValue == 10 || beatmapEventValue == 11 || beatmapEventValue == 12) {
    return EnvironmentColorType::ColorW;
  }
  return EnvironmentColorType::Color0;
}

Sombrero::FastColor GetNormalColorOld(ChromaLightSwitchEventEffect* l, int beatmapEventValue, bool colorBoost) {
  if (colorBoost) {
    if (!Chroma::ChromaLightSwitchEventEffect::IsColor0(beatmapEventValue)) {
      return l->lightColor1Boost->get_color();
    }
    return l->lightColor0Boost->get_color();
  }
  if (!Chroma::ChromaLightSwitchEventEffect::IsColor0(beatmapEventValue)) {
    return l->lightColor1->get_color();
  }
  return l->lightColor0->get_color();
}

void Chroma::ChromaLightSwitchEventEffect::CopyValues(GlobalNamespace::LightSwitchEventEffect* lightSwitchEventEffect) {
  lightColor0 = lightSwitchEventEffect->lightColor0;
  lightColor1 = lightSwitchEventEffect->lightColor1;
  highlightColor0 = lightSwitchEventEffect->highlightColor0;
  highlightColor1 = lightSwitchEventEffect->highlightColor1;
  lightColor0Boost = lightSwitchEventEffect->lightColor0Boost;
  lightColor1Boost = lightSwitchEventEffect->lightColor1Boost;
  highlightColor0Boost = lightSwitchEventEffect->highlightColor0Boost;
  highlightColor1Boost = lightSwitchEventEffect->highlightColor1Boost;

  offColorIntensity = lightSwitchEventEffect->offColorIntensity;
  lightOnStart = lightSwitchEventEffect->lightOnStart;
  this->lightsID = lightSwitchEventEffect->lightsID;
  this->event = lightSwitchEventEffect->event;

  this->lightManager = lightSwitchEventEffect->lightManager;
  this->beatmapCallbacksController = lightSwitchEventEffect->beatmapCallbacksController;
  this->tweeningManager = lightSwitchEventEffect->tweeningManager;
  this->colorManager = lightSwitchEventEffect->colorManager;

  _originalLightColor0 = lightColor0;
  _originalLightColor0Boost = lightColor0Boost;
  _originalLightColor1 = lightColor1;
  _originalLightColor1Boost = lightColor1Boost;

  auto Initialize = [](auto&& so, Sombrero::FastColor& color) {
    if (auto multi = il2cpp_utils::try_cast<MultipliedColorSO>(so)) {
      color = multi.value()->multiplierColor;
    } else {
      color = Sombrero::FastColor::white();
    }
  };

  Initialize(lightColor0, _lightColor0Mult);
  Initialize(lightColor1, _lightColor1Mult);
  Initialize(highlightColor0, _highlightColor0Mult);
  Initialize(highlightColor1, _highlightColor1Mult);
  Initialize(lightColor0Boost, _lightColor0BoostMult);
  Initialize(lightColor1Boost, _lightColor1BoostMult);
  Initialize(highlightColor0Boost, _highlightColor0BoostMult);
  Initialize(highlightColor1Boost, _highlightColor1BoostMult);

  this->lightColorizer = &LightColorizer::New(this, lightManager);
  LightColorizer::CompleteContracts(this);

  //    Sombrero::FastColor color = lightOnStart ? lightColor0->get_color() :
  //    static_cast<Sombrero::FastColor>(lightColor0->get_color()).Alpha(offColorIntensity); SetColor(color);
}

void Chroma::ChromaLightSwitchEventEffect::HandleEvent(GlobalNamespace::BasicBeatmapEventData* beatmapEventData) {
  if (beatmapEventData->basicBeatmapEventType != event) {
    return;
  }
  std::optional<std::vector<ILightWithId*>> selectLights;
  std::optional<Functions> easing;
  std::optional<LerpType> lerpType;

  // fun fun chroma stuff

  static auto contextLogger = getLogger().WithContext(ChromaLogger::ColorLightSwitch);

  auto chromaIt = ChromaEventDataManager::ChromaEventDatas.find(beatmapEventData);

  // Aero thinks legacy was a mistake. I think a Quest port was a bigger mistake.
  std::optional<Sombrero::FastColor> color;

  if (chromaIt == ChromaEventDataManager::ChromaEventDatas.end()) {
    color = LegacyLightHelper::GetLegacyColor(beatmapEventData);
  } else {
    debugSpamLog(contextLogger, "Color is legacy? %s", color ? "true" : "false");

    auto const& chromaData = chromaIt->second;

    auto const& lightMember = chromaData.LightID;
    if (lightMember) {
      auto const& lightIdData = *lightMember;
      selectLights = lightColorizer->GetLightWithIds(lightIdData);
    }

    // Prop ID is deprecated apparently.
    // https://github.com/Aeroluna/Chroma/commit/711cb19f7d03a1776a24cef52fd8ef6fd7685a2b#diff-b8fcfff3ebc4ceb7b43d8401d9f50750dc88326d0a87897c5593923e55b23879R41
    auto const& propMember = chromaData.PropID;
    if (propMember) {
      auto const& propIDData = *propMember;

      selectLights = lightColorizer->GetPropagationLightWithIds(propIDData);
    }

    auto const& gradient = chromaData.GradientObject;
    if (gradient) {
      color = ChromaGradientController::AddGradient(gradient.value(), beatmapEventData->basicBeatmapEventType,
                                                    beatmapEventData->time);
    }

    std::optional<Sombrero::FastColor> const& colorData = chromaData.ColorData;
    if (colorData) {
      color = colorData;
      ChromaGradientController::CancelGradient(beatmapEventData->basicBeatmapEventType);
    }

    easing = chromaData.Easing;
    lerpType = chromaData.LerpType;
  }

  if (color) {
    lightColorizer->Colorize(false, { *color, *color, *color, *color });
  } else if (!ChromaGradientController::IsGradientActive(beatmapEventData->basicBeatmapEventType)) {
    lightColorizer->Colorize(false, { std::nullopt, std::nullopt, std::nullopt, std::nullopt });
  }

  Refresh(true, selectLights, beatmapEventData, easing, lerpType);
}

void ChromaLightSwitchEventEffect::HandleBoostEvent(GlobalNamespace::ColorBoostBeatmapEventData* beatmapEventData) {
  if (beatmapEventData->boostColorsAreOn == usingBoostColors) {
    return;
  }

  usingBoostColors = beatmapEventData->boostColorsAreOn;
  Refresh(false, std::nullopt);
}

void ChromaLightSwitchEventEffect::Awake() {
  livingLightSwitch.emplace(this);
}

void Chroma::ChromaLightSwitchEventEffect::OnDestroy() {
  livingLightSwitch.erase(this);
  static auto const* LightSwitchEventEffect_OnDestroy =
      il2cpp_utils::il2cpp_type_check::MetadataGetter<&LightSwitchEventEffect::OnDestroy>::get();
  il2cpp_utils::RunMethodRethrow<void, false>(this, LightSwitchEventEffect_OnDestroy);

  CRASH_UNLESS(&LightColorizer::Colorizers.at(event) == lightColorizer);
  LightColorizer::Colorizers.erase(event);
}

Sombrero::FastColor Chroma::ChromaLightSwitchEventEffect::GetOriginalColor(int beatmapEventValue,
                                                                           bool colorBoost) const {
  if (colorBoost) {
    return !IsColor0(beatmapEventValue) ? _originalLightColor1Boost->get_color()
                                        : _originalLightColor0Boost->get_color();
  }

  return !IsColor0(beatmapEventValue) ? _originalLightColor1->get_color() : _originalLightColor0->get_color();
}

void ChromaLightSwitchEventEffect::Refresh(bool hard, std::optional<std::vector<ILightWithId*>> const& selectLights,
                                           std::optional<BasicBeatmapEventData*> beatmapEventData,
                                           std::optional<Functions> easing, std::optional<LerpType> lerpType) {
  std::vector<Tween::ChromaColorTweenData*> selectTweens;

  if (selectLights) {
    for (auto const& light : *selectLights) {
      auto tweenIt = ColorTweens.find(light);
      if (tweenIt != ColorTweens.end()) {
        selectTweens.push_back(&tweenIt->second);
      }
    }
  } else {
    for (auto& [_, tween] : ColorTweens) {
      selectTweens.push_back(&tween);
    }
  }

  bool boost = usingBoostColors;
  for (auto const& tweenData : selectTweens) {
    auto const& tween = tweenData->tween;
    CRASH_UNLESS(tween.isHandleValid());
    CRASH_UNLESS(tween.ptr());
    BasicBeatmapEventData* previousEvent = nullptr;
    if (hard) {
      tweenData->PreviousEvent = beatmapEventData.value();
      previousEvent = beatmapEventData.value();
    } else {
      if (tweenData->PreviousEvent == nullptr) {
        // No previous event loaded, cant refresh.
        return;
      }

      previousEvent = tweenData->PreviousEvent;
    }

    int previousValue = previousEvent->value;
    float previousFloatValue = previousEvent->floatValue;

    auto CheckNextEventForFadeBetter = [&, this, hard, easing, lerpType]() {
      auto eventDataIt = ChromaEventDataManager::ChromaEventDatas.find(previousEvent);
      auto const* eventData =
          eventDataIt != ChromaEventDataManager::ChromaEventDatas.end() ? &eventDataIt->second : nullptr;

      auto const& nextSameTypesDict = eventData != nullptr ? &eventData->NextSameTypeEvent : nullptr;

      BasicBeatmapEventData* nextSameTypeEvent = nullptr;
      ChromaEventData* nextEventData = nullptr;
      if (nextSameTypesDict == nullptr) {
        nextSameTypeEvent = static_cast<BasicBeatmapEventData*>(previousEvent->nextSameTypeEventData);
      } else if (nextSameTypesDict->contains(tweenData->id)) {
        auto [anextSameTypeEvent, anextEventData] = nextSameTypesDict->at(tweenData->id);
        nextSameTypeEvent = il2cpp_utils::try_cast<BasicBeatmapEventData>(anextSameTypeEvent).value_or(nullptr);
        // optimization, grab early
        nextEventData = anextEventData;
      }

      if ((nextSameTypeEvent == nullptr) ||
          (nextSameTypeEvent->value != 4 && nextSameTypeEvent->value != 8 && nextSameTypeEvent->value != 12)) {
        return;
      }

      float nextFloatValue = nextSameTypeEvent->floatValue;
      int nextValue = nextSameTypeEvent->value;
      EnvironmentColorType nextColorType = GetLightColorTypeFromEventDataValue(nextSameTypeEvent->value);
      Sombrero::FastColor nextColor;

      eventDataIt = ChromaEventDataManager::ChromaEventDatas.find(nextSameTypeEvent);
      if (nextEventData == nullptr) {
        nextEventData = eventDataIt != ChromaEventDataManager::ChromaEventDatas.end() ? &eventDataIt->second : nullptr;
      }

      std::optional<Sombrero::FastColor> nextColorData =
          nextEventData != nullptr ? nextEventData->ColorData : std::nullopt;
      if (nextColorData) {
        nextColor = nextColorData->Alpha(nextColorData->a * nextColor.a);
      }

      if (nextColorType != EnvironmentColorType::ColorW && nextColorData) {
        Sombrero::FastColor multiplierColor;
        if (usingBoostColors) {
          if (nextColorType == EnvironmentColorType::Color1) {
            multiplierColor = _lightColor1BoostMult;
          } else {
            multiplierColor = _lightColor0BoostMult;
          }
        } else {
          if (nextColorType == EnvironmentColorType::Color1) {
            multiplierColor = _lightColor1Mult;
          } else {
            multiplierColor = _lightColor0Mult;
          }
        }

        nextColor = nextColorData.value() * multiplierColor;
      } else {
        nextColor = GetNormalColorOld(this, nextValue, usingBoostColors);
      }

      nextColor.a *= nextFloatValue;
      Sombrero::FastColor prevColor = tweenData->tween->toValue;
      if (previousValue == 0) {
        prevColor = nextColor.Alpha(0.0F);
      } else if (!IsFixedDurationLightSwitch(previousValue)) {
        prevColor = GetNormalColor(previousValue, boost);
        prevColor.a *= previousFloatValue; // MultAlpha
      }

      tweenData->tween->fromValue = prevColor;
      tweenData->tween->toValue = nextColor;
      tweenData->tween->ForceOnUpdate();

      if (!hard) {
        return;
      }

      tweenData->tween->SetStartTimeAndEndTime(previousEvent->time, nextSameTypeEvent->time);
      tweenData->easing = easing.value_or(Functions::easeLinear);
      tweenData->lerpType = lerpType.value_or(LerpType::RGB);
      tweeningManager->ResumeTween(const_cast<Tweening::ColorTween*>(tween.ptr()), this);
    };

    switch (previousValue) {
    case 0: {
      if (hard) {
        tween->Kill();
      }

      // we just always default color0
      float offAlpha = offColorIntensity * previousFloatValue;
      Color color = GetNormalColor(0, boost);
      color.a = offAlpha; // ColorWithAlpha
      tween->fromValue = color;
      tween->toValue = color;
      Chroma::Tween::SetColor(tweenData->lightWithId.ptr(), lightManager, color);
      CheckNextEventForFadeBetter();

      break;
    }

    case 1:
    case 5:
    case 9:
    case 4:
    case 8:
    case 12: {
      if (hard) {
        tween->Kill();
      }

      Color color = GetNormalColor(previousValue, boost);
      color.a *= previousFloatValue; // MultAlpha
      tween->fromValue = color;
      tween->toValue = color;
      Chroma::Tween::SetColor(tweenData->lightWithId.ptr(), lightManager, color);
      CheckNextEventForFadeBetter();
      break;
    }

    case 2:
    case 6:
    case 10: {
      Color colorFrom = GetHighlightColor(previousValue, boost);
      colorFrom.a *= previousFloatValue; // MultAlpha
      Color colorTo = GetNormalColor(previousValue, boost);
      colorTo.a *= previousFloatValue; // MultAlpha
      tween->fromValue = colorFrom;
      tween->toValue = colorTo;
      tween->ForceOnUpdate();

      if (hard) {
        tween->duration = 0.6F;
        tweenData->easing = easing.value_or(Functions::easeOutCubic);
        tweenData->lerpType = lerpType.value_or(LerpType::RGB);
        tweeningManager->RestartTween(const_cast<Tweening::ColorTween*>(tween.ptr()), this);
      }

      break;
    }

    case 3:
    case 7:
    case 11:
    case -1: {
      Color colorFrom = GetHighlightColor(previousValue, boost);
      colorFrom.a *= previousFloatValue; // MultAlpha

      Color colorTo = GetNormalColor(previousValue, boost);
      colorTo.a = offColorIntensity * previousFloatValue; // ColorWithAlpha
      tween->fromValue = colorFrom;
      tween->toValue = colorTo;
      tween->ForceOnUpdate();

      if (hard) {
        tween->duration = 1.5F;
        tweenData->easing = easing.value_or(Functions::easeOutExpo);
        tweenData->lerpType = lerpType.value_or(LerpType::RGB);
        tweeningManager->RestartTween(const_cast<Tweening::ColorTween*>(tween.ptr()), this);
      }

      break;
    }
    }
  }
}

// improve speed, avoid codegen
constexpr bool ChromaLightSwitchEventEffect::IsColor0(int beatmapEventValue) {
  return beatmapEventValue == 1 || beatmapEventValue == 2 || beatmapEventValue == 3 || beatmapEventValue == 4 ||
         beatmapEventValue == 0 || beatmapEventValue == -1;
}

Sombrero::FastColor ChromaLightSwitchEventEffect::GetNormalColor(int beatmapEventValue, bool colorBoost) {
  switch (GetLightColorTypeFromEventDataValue(beatmapEventValue)) {
  default: {
    if (colorBoost) {
      return lightColorizer->getColor()[2] * _lightColor0BoostMult;
    }
    return lightColorizer->getColor()[0] * _lightColor0Mult;
  }
  case EnvironmentColorType::Color1: {
    if (colorBoost) {
      return lightColorizer->getColor()[3] * _lightColor1BoostMult;
    }
    return lightColorizer->getColor()[1] * _lightColor1Mult;
  }
  case EnvironmentColorType::ColorW: {
    return colorManager->ColorForType(EnvironmentColorType::ColorW, colorBoost);
  }
  }
}

Sombrero::FastColor ChromaLightSwitchEventEffect::GetHighlightColor(int beatmapEventValue, bool colorBoost) {
  switch (GetLightColorTypeFromEventDataValue(beatmapEventValue)) {
  default: {
    if (colorBoost) {
      return lightColorizer->getColor()[2] * _highlightColor0BoostMult;
    }
    return lightColorizer->getColor()[0] * _highlightColor0Mult;
  }
  case EnvironmentColorType::Color1: {
    if (colorBoost) {
      return lightColorizer->getColor()[3] * _highlightColor1BoostMult;
    }
    return lightColorizer->getColor()[1] * _highlightColor1Mult;
  }
  case EnvironmentColorType::ColorW: {
    return colorManager->ColorForType(EnvironmentColorType::ColorW, colorBoost);
  }
  }
}

constexpr bool ChromaLightSwitchEventEffect::IsFixedDurationLightSwitch(int beatmapEventValue) {
  return beatmapEventValue == 2 || beatmapEventValue == 6 || beatmapEventValue == 10 || beatmapEventValue == 3 ||
         beatmapEventValue == 7 || beatmapEventValue == 11 || beatmapEventValue == -1;
}

void ChromaLightSwitchEventEffect::UnregisterLight(GlobalNamespace::ILightWithId* lightWithId) {
  auto it = ColorTweens.find(lightWithId);

  if (it == ColorTweens.end()) {
    return;
  }

  auto& tweenData = it->second;

  ColorTweensMapping.erase(tweenData.tween.ptr());

  tweenData.tween->Kill();
  ColorTweens.erase(it);
}

void ChromaLightSwitchEventEffect::RegisterLight(GlobalNamespace::ILightWithId* lightWithId, int id) {
  if (ColorTweens.contains(lightWithId)) {
    return;
  }

  Sombrero::FastColor color = GetNormalColor(0, usingBoostColors);
  if (!lightOnStart) {
    color = color.Alpha(offColorIntensity);
  }

  auto tableId = LightIDTableManager::GetActiveTableValueReverse(lightsID, id).value_or(0);
  auto* tween = Chroma::Tween::makeTween(color, color, lightWithId, lightManager);

  auto it = ColorTweens.try_emplace(lightWithId, tableId, tween, lightWithId);
  ColorTweensMapping[tween] = &it.first->second;

  tween->ForceOnUpdate();
}
