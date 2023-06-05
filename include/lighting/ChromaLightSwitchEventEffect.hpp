#pragma once
#include <vector>
#include <string>
#include <optional>
#include <unordered_map>

#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

#include "UnityEngine/Color.hpp"

#include "GlobalNamespace/ColorBoostBeatmapEventData.hpp"
#include "GlobalNamespace/BasicBeatmapEventType.hpp"
#include "GlobalNamespace/ILightWithId.hpp"
#include "GlobalNamespace/LightWithIdManager.hpp"
#include "GlobalNamespace/BasicBeatmapEventData.hpp"
#include "GlobalNamespace/LightSwitchEventEffect.hpp"
#include "GlobalNamespace/ColorSO.hpp"

#include "ChromaController.hpp"
#include "ColorTween.hpp"

#include "sombrero/shared/ColorUtils.hpp"
#include "tracks/shared/Animation/Easings.h"
#include "main.hpp"

namespace Chroma {
    class LightColorizer;
}


DECLARE_CLASS_CODEGEN(Chroma, ChromaLightSwitchEventEffect, GlobalNamespace::LightSwitchEventEffect,
public:
          DECLARE_INSTANCE_FIELD(GlobalNamespace::ColorSO*, _originalLightColor0);
          DECLARE_INSTANCE_FIELD(GlobalNamespace::ColorSO*, _originalLightColor1);
          DECLARE_INSTANCE_FIELD(GlobalNamespace::ColorSO*, _originalLightColor0Boost);
          DECLARE_INSTANCE_FIELD(GlobalNamespace::ColorSO*, _originalLightColor1Boost);
//          std::array<Sombrero::FastColor, 4> OriginalColors;

        Sombrero::FastColor _lightColor0Mult;
        Sombrero::FastColor _lightColor1Mult;
        Sombrero::FastColor _highlightColor0Mult;
        Sombrero::FastColor _highlightColor1Mult;
        Sombrero::FastColor _lightColor0BoostMult;
        Sombrero::FastColor _lightColor1BoostMult;
        Sombrero::FastColor _highlightColor0BoostMult;
        Sombrero::FastColor _highlightColor1BoostMult;

          std::unordered_map<GlobalNamespace::ILightWithId *, Tween::ChromaColorTweenData> ColorTweens;
          static std::unordered_map<Tweening::ColorTween *, Tween::ChromaColorTweenData*> ColorTweensMapping;

          GlobalNamespace::BasicBeatmapEventType EventType;

          Chroma::LightColorizer* lightColorizer;

          DECLARE_INSTANCE_METHOD(void, Awake);
          DECLARE_INSTANCE_METHOD(void, OnDestroy);
          DECLARE_SIMPLE_DTOR();
          DECLARE_DEFAULT_CTOR();

public:
    static std::unordered_set<ChromaLightSwitchEventEffect*> livingLightSwitch;

    // improve speed, avoid codegen
    Sombrero::FastColor GetNormalColor(int beatmapEventValue, bool colorBoost);
    Sombrero::FastColor GetHighlightColor(int beatmapEventValue, bool colorBoost);
    static constexpr bool IsFixedDurationLightSwitch(int beatmapEventValue);
    static constexpr bool IsColor0(int beatmapEventValue);

          void HandleEvent(GlobalNamespace::BasicBeatmapEventData* BasicBeatmapEventData);
          void HandleBoostEvent(GlobalNamespace::ColorBoostBeatmapEventData* BasicBeatmapEventData);

          void Refresh(bool hard, std::optional<std::vector<GlobalNamespace::ILightWithId*>> const& selectLights, std::optional<GlobalNamespace::BasicBeatmapEventData*> BasicBeatmapEventData = std::nullopt, std::optional<Functions> easing = std::nullopt, std::optional<LerpType> lerpType = std::nullopt);

          void CopyValues(LightSwitchEventEffect* lightSwitchEventEffect);
public:
          void RegisterLight(GlobalNamespace::ILightWithId* lightWithId, int id);
          void UnregisterLight(GlobalNamespace::ILightWithId* lightWithId);
          Sombrero::FastColor GetOriginalColor(int beatmapEventValue, bool colorBoost) const;

)

