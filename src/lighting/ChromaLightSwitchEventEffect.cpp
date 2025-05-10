#include "lighting/ChromaLightSwitchEventEffect.hpp"
#include "GlobalNamespace/BeatmapDataItem.hpp"
#include "System/Tuple_2.hpp"
#include "lighting/ChromaEventData.hpp"
#include "lighting/LegacyLightHelper.hpp"
#include "lighting/ChromaGradientController.hpp"

#include "GlobalNamespace/EnvironmentColorType.hpp"
#include "GlobalNamespace/ColorManager.hpp"
#include "GlobalNamespace/ColorBoostBeatmapEventData.hpp"

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
  switch (GetLightColorTypeFromEventDataValue(beatmapEventValue)) {
  case EnvironmentColorType::Color0:
    if (!colorBoost) {
      return l->_lightColor0->color;
    }
    return l->_lightColor0Boost->color;
  case EnvironmentColorType::Color1:
    if (!colorBoost) {
      return l->_lightColor1->color;
    }
    return l->_lightColor1Boost->color;
  case EnvironmentColorType::ColorW:
    return l->_colorManager->ColorForType(EnvironmentColorType::ColorW, colorBoost);
  default:
    return l->_lightColor0->color;
  }
}

void Chroma::ChromaLightSwitchEventEffect::CopyValues(GlobalNamespace::LightSwitchEventEffect* lightSwitchEventEffect) {
  _lightColor0 = lightSwitchEventEffect->_lightColor0;
  _lightColor1 = lightSwitchEventEffect->_lightColor1;
  _highlightColor0 = lightSwitchEventEffect->_highlightColor0;
  _highlightColor1 = lightSwitchEventEffect->_highlightColor1;
  _lightColor0Boost = lightSwitchEventEffect->_lightColor0Boost;
  _lightColor1Boost = lightSwitchEventEffect->_lightColor1Boost;
  _highlightColor0Boost = lightSwitchEventEffect->_highlightColor0Boost;
  _highlightColor1Boost = lightSwitchEventEffect->_highlightColor1Boost;

  _offColorIntensity = lightSwitchEventEffect->_offColorIntensity;
  _lightOnStart = lightSwitchEventEffect->_lightOnStart;
  this->_lightsID = lightSwitchEventEffect->_lightsID;
  this->_event = lightSwitchEventEffect->_event;

  this->_lightManager = lightSwitchEventEffect->_lightManager;
  this->_beatmapCallbacksController = lightSwitchEventEffect->_beatmapCallbacksController;
  this->_tweeningManager = lightSwitchEventEffect->_tweeningManager;
  this->_colorManager = lightSwitchEventEffect->_colorManager;

  _originalLightColor0 = _lightColor0;
  _originalLightColor0Boost = _lightColor0Boost;
  _originalLightColor1 = _lightColor1;
  _originalLightColor1Boost = _lightColor1Boost;

  auto Initialize = [](UnityW<ColorSO> so, Sombrero::FastColor& color) {
    if (auto multi = il2cpp_utils::try_cast<MultipliedColorSO>(so.ptr())) {
      color = multi.value()->_multiplierColor;
    } else {
      color = Sombrero::FastColor::white();
    }
  };

  Initialize(_lightColor0, _lightColor0Mult);
  Initialize(_lightColor1, _lightColor1Mult);
  Initialize(_highlightColor0, _highlightColor0Mult);
  Initialize(_highlightColor1, _highlightColor1Mult);
  Initialize(_lightColor0Boost, _lightColor0BoostMult);
  Initialize(_lightColor1Boost, _lightColor1BoostMult);
  Initialize(_highlightColor0Boost, _highlightColor0BoostMult);
  Initialize(_highlightColor1Boost, _highlightColor1BoostMult);

  this->lightColorizer = &LightColorizer::New(this, _lightManager);
  LightColorizer::CompleteContracts(this);

  //    Sombrero::FastColor color = lightOnStart ? lightColor0->get_color() :
  //    static_cast<Sombrero::FastColor>(lightColor0->get_color()).Alpha(offColorIntensity); SetColor(color);
}

void Chroma::ChromaLightSwitchEventEffect::HandleEvent(GlobalNamespace::BasicBeatmapEventData* beatmapEventData) {
  if (beatmapEventData->basicBeatmapEventType != _event) {
    return;
  }
  std::optional<std::vector<ILightWithId*>> selectLights;
  std::optional<Functions> easing;
  std::optional<LerpType> lerpType;

  // fun fun chroma stuff

  auto chromaIt = ChromaEventDataManager::ChromaEventDatas.find(beatmapEventData);

  // Aero thinks legacy was a mistake. I think a Quest port was a bigger mistake.
  std::optional<Sombrero::FastColor> color;

  if (chromaIt == ChromaEventDataManager::ChromaEventDatas.end()) {
    color = LegacyLightHelper::GetLegacyColor(beatmapEventData);
  } else {
    debugSpamLog("Color is legacy? {}", color ? "true" : "false");

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
  if (beatmapEventData->boostColorsAreOn == ____usingBoostColors) {
    return;
  }

  ____usingBoostColors = beatmapEventData->boostColorsAreOn;
  Refresh(false, std::nullopt);
}

void ChromaLightSwitchEventEffect::Awake() {
  livingLightSwitch.emplace(this);
}

void Chroma::ChromaLightSwitchEventEffect::OnDestroy() {
  livingLightSwitch.erase(this);
  static auto const* LightSwitchEventEffect_OnDestroy =
      il2cpp_utils::il2cpp_type_check::MetadataGetter<&LightSwitchEventEffect::OnDestroy>::methodInfo();
  il2cpp_utils::RunMethodRethrow<void, false>(this, LightSwitchEventEffect_OnDestroy);

  CRASH_UNLESS(&LightColorizer::Colorizers.at(_event.value__) == lightColorizer);
  LightColorizer::Colorizers.erase(_event.value__);
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

  bool boost = _usingBoostColors;
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
        auto [tempNextSameTypeEvent, tempNextEventData] = nextSameTypesDict->at(tweenData->id);
        // optimization, grab early
        nextEventData = tempNextEventData;

        nextSameTypeEvent = il2cpp_utils::try_cast<BasicBeatmapEventData>(tempNextSameTypeEvent).value_or(nullptr);

      } else if (nextSameTypesDict->contains(-1)) {
        auto [tempNextSameTypeEvent, tempNextEventData] = nextSameTypesDict->at(-1);
        // optimization, grab early
        nextEventData = tempNextEventData;
        nextSameTypeEvent = il2cpp_utils::try_cast<BasicBeatmapEventData>(tempNextSameTypeEvent).value_or(nullptr);
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
        if (_usingBoostColors) {
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
        nextColor = GetNormalColorOld(this, nextValue, _usingBoostColors);
      }

      nextColor.a *= nextFloatValue;
      Sombrero::FastColor prevColor = tweenData->tween->toValue;
      if (previousValue == 0) {
        prevColor = nextColor.Alpha(0.0F);
      } else if (!IsFixedDurationLightSwitch(previousValue)) {
        prevColor = GetNormalColor(previousValue, boost);
        prevColor.a *= previousFloatValue; // MultAlpha
      }

      System::Collections::Generic::List_1<float>::New_ctor()->_items = ArrayW<float>::Empty();

      auto x = System::Nullable_1<float>();
      x.value = 5;

      auto y = System::Tuple_2<float, float>::New_ctor(0, 0);
      y->m_Item1 = 5;

      Tweening::Tween_1<float>::New_ctor()->toValue = 2;

      tweenData->tween->fromValue = prevColor;
      tweenData->tween->toValue = nextColor;
      tweenData->tween->ForceOnUpdate();

      if (!hard) {
        return;
      }

      tweenData->tween->SetStartTimeAndEndTime(previousEvent->time, nextSameTypeEvent->time);
      tweenData->easing = easing.value_or(Functions::EaseLinear);
      tweenData->lerpType = lerpType.value_or(LerpType::RGB);
      _tweeningManager->ResumeTween(const_cast<Tweening::ColorTween*>(tween.ptr()), this, false);
    };

    switch (previousValue) {
    case 0: {
      if (hard) {
        tween->Kill();
      }

      // we just always default color0
      float offAlpha = _offColorIntensity * previousFloatValue;
      Color color = GetNormalColor(0, boost);
      color.a = offAlpha; // ColorWithAlpha
      tween->___fromValue = color;
      tween->___toValue = color;
      Chroma::Tween::SetColor(tweenData->lightWithId.ptr(), _lightManager, color);
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
      tween->___fromValue = color;
      tween->___toValue = color;
      Chroma::Tween::SetColor(tweenData->lightWithId.ptr(), _lightManager, color);
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
      tween->___fromValue = colorFrom;
      tween->___toValue = colorTo;
      tween->ForceOnUpdate();

      if (hard) {
        tween->duration = 0.6F;
        tweenData->easing = easing.value_or(Functions::EaseOutCubic);
        tweenData->lerpType = lerpType.value_or(LerpType::RGB);
        _tweeningManager->RestartTween(const_cast<Tweening::ColorTween*>(tween.ptr()), this, false);
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
      colorTo.a = _offColorIntensity * previousFloatValue; // ColorWithAlpha
      tween->___fromValue = colorFrom;
      tween->___toValue = colorTo;
      tween->ForceOnUpdate();

      if (hard) {
        tween->duration = 1.5F;
        tweenData->easing = easing.value_or(Functions::EaseOutExpo);
        tweenData->lerpType = lerpType.value_or(LerpType::RGB);
        _tweeningManager->RestartTween(const_cast<Tweening::ColorTween*>(tween.ptr()), this, false);
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
    return _colorManager->ColorForType(EnvironmentColorType::ColorW, colorBoost);
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
    return _colorManager->ColorForType(EnvironmentColorType::ColorW, colorBoost);
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

  Sombrero::FastColor color = GetNormalColor(0, _usingBoostColors);
  if (!_lightOnStart) {
    color = color.Alpha(_offColorIntensity);
  }

  auto tableId = LightIDTableManager::GetActiveTableValueReverse(_lightsID, id).value_or(0);
  auto* tween = Chroma::Tween::makeTween(color, color, lightWithId, _lightManager);

  auto it = ColorTweens.try_emplace(lightWithId, tableId, tween, lightWithId);
  ColorTweensMapping[tween] = &it.first->second;

  tween->ForceOnUpdate();
}
