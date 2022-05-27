#include "lighting/ChromaLightSwitchEventEffect.hpp"
#include "lighting/ChromaEventData.hpp"
#include "lighting/LegacyLightHelper.hpp"
#include "lighting/ChromaGradientController.hpp"

#include "Tweening/SongTimeTweeningManager.hpp"
#include "lighting/LightIDTableManager.hpp"
#include "colorizer/LightColorizer.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;

DEFINE_TYPE(Chroma, ChromaLightSwitchEventEffect);

std::unordered_set<ChromaLightSwitchEventEffect*> ChromaLightSwitchEventEffect::livingLightSwitch;

void Chroma::ChromaLightSwitchEventEffect::CopyValues(GlobalNamespace::LightSwitchEventEffect *lightSwitchEventEffect) {
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
    lightsID = lightSwitchEventEffect->lightsID;
    event = lightSwitchEventEffect->event;

    this->lightManager = lightSwitchEventEffect->lightManager;
    this->beatmapCallbacksController = lightSwitchEventEffect->beatmapCallbacksController;
    this->tweeningManager = lightSwitchEventEffect->tweeningManager;

    this->lightColorizer = &LightColorizer::New(this, lightManager);
    _originalLightColor0 = lightColor0;
    _originalLightColor0Boost = lightColor0Boost;
    _originalLightColor1 = lightColor1;
    _originalLightColor1Boost = lightColor1Boost;
    lightColorizer->InitializeSO(lightColor0, highlightColor0, lightColor1, highlightColor1, lightColor0Boost, highlightColor0Boost, lightColor1Boost, highlightColor1Boost);

    auto const& lights = lightColorizer->Lights;
    for (int i = 0; i < lights.size(); i++)
    {
        RegisterLight(lights[i], (int)event, i);
    }

    Sombrero::FastColor color = lightOnStart ? lightColor0->get_color() : static_cast<Sombrero::FastColor>(lightColor0->get_color()).Alpha(offColorIntensity);
    SetColor(color);
}

void Chroma::ChromaLightSwitchEventEffect::HandleEvent(GlobalNamespace::BasicBeatmapEventData *beatmapEventData) {
    if (beatmapEventData->basicBeatmapEventType == event) {
        std::optional<std::vector<ILightWithId *>> selectLights;
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
                auto const &lightIdData = *lightMember;
                selectLights = lightColorizer->GetLightWithIds(lightIdData);
            }



            // Prop ID is deprecated apparently.  https://github.com/Aeroluna/Chroma/commit/711cb19f7d03a1776a24cef52fd8ef6fd7685a2b#diff-b8fcfff3ebc4ceb7b43d8401d9f50750dc88326d0a87897c5593923e55b23879R41
            auto const& propMember = chromaData.PropID;
            if (propMember) {
                auto const &propIDData = *propMember;

                selectLights = lightColorizer->GetPropagationLightWithIds(propIDData);
            }


            auto const& gradient = chromaData.GradientObject;
            if (gradient) {
                color = ChromaGradientController::AddGradient(gradient.value(), beatmapEventData->basicBeatmapEventType,
                                                              beatmapEventData->time);
            }


            std::optional<Sombrero::FastColor> const &colorData = chromaData.ColorData;
            if (colorData) {
                color = colorData;
                ChromaGradientController::CancelGradient(beatmapEventData->basicBeatmapEventType);
            }

            easing = chromaData.Easing;
            lerpType = chromaData.LerpType;
        }


        if (color) {
            lightColorizer->Colorize(false, {*color, *color, *color, *color});
        } else if (!ChromaGradientController::IsGradientActive(beatmapEventData->basicBeatmapEventType)) {
            lightColorizer->Colorize(false,
                                     {std::nullopt, std::nullopt, std::nullopt, std::nullopt});
        }




        Refresh(true, selectLights, beatmapEventData, easing, lerpType);
    }
}

void ChromaLightSwitchEventEffect::HandleBoostEvent(GlobalNamespace::ColorBoostBeatmapEventData *beatmapEventData) {
    if(beatmapEventData->boostColorsAreOn == usingBoostColors) {
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
    static auto LightSwitchEventEffect_OnDestroy = il2cpp_utils::il2cpp_type_check::MetadataGetter<&LightSwitchEventEffect::OnDestroy>::get();
    il2cpp_utils::RunMethodRethrow<void, false>(this, LightSwitchEventEffect_OnDestroy);

    CRASH_UNLESS(&LightColorizer::Colorizers.at(event) == lightColorizer);
    LightColorizer::Colorizers.erase(event);
}

void Chroma::ChromaLightSwitchEventEffect::RegisterLight(GlobalNamespace::ILightWithId *lightWithId, int type, int id) {
    using namespace Sombrero;
    if (!ColorTweens.contains(lightWithId))
    {
        // https://github.com/Aeroluna/Heck/commit/985cab1b53ea8cc2f6ca94f38080ed79dbe7eead#diff-4879964c1cc52000f170a1b271d1569746571c53454b82c360fa92a85060ccd8L382
        ColorTweens[lightWithId] = CRASH_UNLESS(il2cpp_utils::New<ChromaIDColorTween*>(FastColor::black(), FastColor::black(), lightWithId, lightManager, LightIDTableManager::GetActiveTableValueReverse(this->lightsID, id).value_or(0)));
    }
    else
    {
        getLogger().error("Attempted to register duplicate ILightWithId.");
    }
}

Sombrero::FastColor
Chroma::ChromaLightSwitchEventEffect::GetOriginalColor(int beatmapEventValue, bool colorBoost) {
    if (colorBoost)
    {
        return !IsColor0(beatmapEventValue) ? _originalLightColor1Boost->get_color() : _originalLightColor0Boost->get_color();
    }

    return !IsColor0(beatmapEventValue) ? _originalLightColor1->get_color() : _originalLightColor0->get_color();
}


void ChromaLightSwitchEventEffect::Refresh(bool hard, const std::optional<std::vector<ILightWithId *>> &selectLights,
                                           std::optional<BasicBeatmapEventData *> beatmapEventData,
                                           std::optional<Functions> easing, std::optional<LerpType> lerpType) {
    std::vector<ChromaIDColorTween*> selectTweens;

    if (selectLights) {
        for (auto light : *selectLights) {
            auto tweenIt = ColorTweens.find(light);
            if (tweenIt != ColorTweens.end()) {
                selectTweens.push_back(static_cast<ChromaIDColorTween *>(tweenIt->second));
            }
        }
    } else {
        for (auto const& [_, tween] : ColorTweens) {
            selectTweens.push_back(static_cast<ChromaIDColorTween*>(tween));
        }
    }


    bool boost = usingBoostColors;
    for (auto const& tween : selectTweens) {
        BasicBeatmapEventData* previousEvent;
        if (hard) {
            tween->PreviousEvent = beatmapEventData.value();
            previousEvent = beatmapEventData.value();
        } else {
            if (tween->PreviousEvent == nullptr) {
                // No previous event loaded, cant refresh.
                return;
            }

            previousEvent = tween->PreviousEvent;
        }

        int previousValue = previousEvent->value;
        float previousFloatValue = previousEvent->floatValue;

        auto CheckNextEventForFadeBetter = [&previousEvent, &tween, this, &boost, &previousValue, &previousFloatValue, hard, easing, lerpType](){
            auto eventDataIt = ChromaEventDataManager::ChromaEventDatas.find(previousEvent);
            auto const* eventData = eventDataIt != ChromaEventDataManager::ChromaEventDatas.end() ? &eventDataIt->second : nullptr;

            BasicBeatmapEventData* nextSameTypeEvent;
            ChromaEventData* nextEventData = nullptr;
            if (eventData && eventData->NextSameTypeEvent.contains(tween->Id))
            {
                auto [anextSameTypeEvent, anextEventData] = eventData->NextSameTypeEvent.at(tween->Id);
                nextSameTypeEvent = anextSameTypeEvent;
                nextEventData = anextEventData;
            }
            else
            {
                nextSameTypeEvent = static_cast<BasicBeatmapEventData *>(previousEvent->nextSameTypeEventData);
            }

            if (!nextSameTypeEvent || (nextSameTypeEvent->value != 4 && nextSameTypeEvent->value != 8))
            {
                return;
            }

            float nextFloatValue = nextSameTypeEvent->floatValue;
            int nextValue = nextSameTypeEvent->value;
            Sombrero::FastColor nextColor = GetOriginalColor(nextValue, boost);

            eventDataIt = ChromaEventDataManager::ChromaEventDatas.find(nextSameTypeEvent);
            if (!nextEventData)
                nextEventData = eventDataIt != ChromaEventDataManager::ChromaEventDatas.end() ? &eventDataIt->second : nullptr;

            std::optional<Sombrero::FastColor> nextColorData = nextEventData ? nextEventData->ColorData : std::nullopt;
            if (nextColorData) {
                nextColor = nextColorData->Alpha(nextColorData->a * nextColor.a);
            }

            nextColor.a *= nextFloatValue;
            Sombrero::FastColor prevColor = tween->toValue;
            if (previousValue == 0) {
                prevColor = nextColor.Alpha(0.0f);
            } else if (!IsFixedDurationLightSwitch(previousValue)) {
                prevColor = GetNormalColor(previousValue, boost);
                prevColor.a *= previousFloatValue; // MultAlpha
            }

            tween->fromValue = prevColor;
            tween->toValue = nextColor;
            tween->ForceOnUpdate();

            if (!hard) {
                return;
            }

            tween->SetStartTimeAndEndTime(previousEvent->time, nextSameTypeEvent->time);
            tween->easing = easing.value_or(Functions::easeLinear);
            tween->lerpType = lerpType.value_or(LerpType::RGB);
            tweeningManager->ResumeTween(tween, this);
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
                tween->SetColor(color);
                CheckNextEventForFadeBetter();

                break;
            }
            case 1:
            case 5:
            case 4:
            case 8: {
                if (hard) {
                    tween->Kill();
                }

                Color color = GetNormalColor(previousValue, boost);
                color.a *= previousFloatValue; // MultAlpha
                tween->fromValue = color;
                tween->toValue = color;
                tween->SetColor(color);
                CheckNextEventForFadeBetter();
                break;
            }



            case 2:
            case 6: {
                Color colorFrom = GetHighlightColor(previousValue, boost);
                colorFrom.a *= previousFloatValue; // MultAlpha
                Color colorTo = GetNormalColor(previousValue, boost);
                colorTo.a *= previousFloatValue; // MultAlpha
                tween->fromValue = colorFrom;
                tween->toValue = colorTo;
                tween->ForceOnUpdate();

                if (hard) {
                    tween->duration = 0.6f;
                    tween->easing = easing.value_or(Functions::easeOutCubic);
                    tween->lerpType = lerpType.value_or(LerpType::RGB);
                    tweeningManager->RestartTween(tween, this);
                }

                break;
            }

            case 3:
            case 7:
            case -1: {
                Color colorFrom = GetHighlightColor(previousValue, boost);
                colorFrom.a *= previousFloatValue; // MultAlpha

                Color colorTo = GetNormalColor(previousValue, boost);
                colorTo.a = offColorIntensity * previousFloatValue; // ColorWithAlpha
                tween->fromValue = colorFrom;
                tween->toValue = colorTo;
                tween->ForceOnUpdate();

                if (hard) {
                    tween->duration = 1.5f;
                    tween->easing = easing.value_or(Functions::easeOutExpo);
                    tween->lerpType = lerpType.value_or(LerpType::RGB);
                    tweeningManager->RestartTween(tween, this);
                }

                break;
            }
        }
    }
}

// improve speed, avoid codegen
constexpr bool ChromaLightSwitchEventEffect::IsColor0(int beatmapEventValue) {
    return beatmapEventValue == 1 || beatmapEventValue == 2 || beatmapEventValue == 3 || beatmapEventValue == 4 || beatmapEventValue == 0 || beatmapEventValue == -1;
}

Sombrero::FastColor ChromaLightSwitchEventEffect::GetNormalColor(int beatmapEventValue, bool colorBoost) {
    if (colorBoost)
    {
        if (!this->IsColor0(beatmapEventValue))
        {
            return this->lightColor1Boost->get_color();
        }
        return this->lightColor0Boost->get_color();
    }
    else
    {
        if (!this->IsColor0(beatmapEventValue))
        {
            return this->lightColor1->get_color();
        }
        return this->lightColor0->get_color();
    }
}

Sombrero::FastColor ChromaLightSwitchEventEffect::GetHighlightColor(int beatmapEventValue, bool colorBoost) {
    if (colorBoost)
    {
        if (!this->IsColor0(beatmapEventValue))
        {
            return this->highlightColor1Boost->get_color();
        }
        return this->highlightColor0Boost->get_color();
    }
    else
    {
        if (!this->IsColor0(beatmapEventValue))
        {
            return this->highlightColor1->get_color();
        }
        return this->highlightColor0->get_color();
    }
}

constexpr bool ChromaLightSwitchEventEffect::IsFixedDurationLightSwitch(int beatmapEventValue) {
    return beatmapEventValue == 2 || beatmapEventValue == 6 || beatmapEventValue == 3 || beatmapEventValue == 7 || beatmapEventValue == -1;
}
