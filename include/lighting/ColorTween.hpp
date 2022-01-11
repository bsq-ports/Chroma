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
#include "Tweening/ColorTween.hpp"

#include "ChromaController.hpp"
#include "LerpType.hpp"

#include "sombrero/shared/ColorUtils.hpp"
#include "sombrero/shared/HSBColor.hpp"
#include "tracks/shared/Animation/Easings.h"
#include "main.hpp"


DECLARE_CLASS_CODEGEN(Chroma, ChromaIDColorTween, Tweening::ColorTween,

public:
                      DECLARE_INSTANCE_FIELD(GlobalNamespace::ILightWithId*, _lightWithId);
                      DECLARE_INSTANCE_FIELD(GlobalNamespace::LightWithIdManager*, _lightWithIdManager);
                      DECLARE_INSTANCE_FIELD(GlobalNamespace::BeatmapEventData*, PreviousEvent);
public:
                      int Id;
                      Functions easing;
                      LerpType lerpType;
    DECLARE_SIMPLE_DTOR();

    DECLARE_CTOR(ctor, Sombrero::FastColor fromValue, Sombrero::FastColor toValue, GlobalNamespace::ILightWithId* lightWithId, GlobalNamespace::LightWithIdManager* lightWithIdManager, int id);

public:
    Sombrero::FastColor GetColor(float time) {
      time = Easings::Interpolate(time, easing);
      switch (lerpType) {
          case LerpType::RGB:
          return Sombrero::FastColor::LerpUnclamped(fromValue, toValue, time);

          case LerpType::HSV:
              float fromH, fromS, fromV;
              float toH, toS, toV;
              Sombrero::FastColor::RGBToHSV(fromValue, fromH, fromS, fromV);
              Sombrero::FastColor::RGBToHSV(toValue, toH, toS, toV);

              return Sombrero::FastColor::HSVToRGB(std::lerp(fromH, toH, time), std::lerp(fromS, toS, time), std::lerp(fromV, toV, time)).Alpha(std::lerp(fromValue.a, toValue.a, time));

          default:
              throw std::runtime_error("Lerp not valid");
      }
    }

    void SetColor(UnityEngine::Color const& color) const {
        _lightWithIdManager->didChangeSomeColorsThisFrame = true;
        if (_lightWithId->get_isRegistered()) {
            _lightWithId->ColorWasSet(color);
        }
    }

)

