#pragma once
#include <vector>
#include <string>
#include <optional>
#include <unordered_map>

#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

#include "UnityEngine/Color.hpp"

#include "GlobalNamespace/BeatmapEventType.hpp"
#include "GlobalNamespace/ILightWithId.hpp"
#include "GlobalNamespace/LightWithIdManager.hpp"
#include "GlobalNamespace/BeatmapEventData.hpp"
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

          std::unordered_map<GlobalNamespace::ILightWithId *, SafePtr<ChromaIDColorTween>> ColorTweens;

          GlobalNamespace::BeatmapEventType EventType;

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

          void HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(GlobalNamespace::BeatmapEventData* beatmapEventData);
          void Refresh(bool hard, std::optional<std::vector<GlobalNamespace::ILightWithId*>> const& selectLights, std::optional<GlobalNamespace::BeatmapEventData*> beatmapEventData = std::nullopt, std::optional<Functions> easing = std::nullopt, std::optional<LerpType> lerpType = std::nullopt);

          void CopyValues(LightSwitchEventEffect* lightSwitchEventEffect);
          void RegisterLight(GlobalNamespace::ILightWithId* lightWithId, int type, int id);
          Sombrero::FastColor GetOriginalColor(int beatmapEventValue, bool colorBoost);

)

