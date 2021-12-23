#pragma once

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include "tracks/shared/Animation/Easings.h"

#include <unordered_map>

#include "UnityEngine/Color.hpp"
#include "GlobalNamespace/BeatmapEventData.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"

#include "sombrero/shared/ColorUtils.hpp"

namespace Chroma {
    class ChromaEventData {
    public:
        std::optional<std::reference_wrapper<const rapidjson::Value>> LightID;

        std::optional<std::reference_wrapper<const rapidjson::Value>> PropID;

        std::optional<Sombrero::FastColor> ColorData;

        struct GradientObjectData {
            float Duration;

            Sombrero::FastColor StartColor;

            Sombrero::FastColor EndColor;

            Functions Easing;
        };

        std::optional<GradientObjectData> GradientObject;

        // ChromaRingRotationEventData

        std::optional<std::string> NameFilter;

        std::optional<int> Direction;

        std::optional<bool> CounterSpin;

        std::optional<bool> Reset;

        std::optional<float> Step;

        std::optional<float> Prop;

        std::optional<float> Speed;

        std::optional<float> Rotation;

        float StepMult;

        float PropMult;

        float SpeedMult;

        // ChromaLaserSpeedEventData

        bool LockPosition;
    };

    class ChromaEventDataManager {
    public:
        typedef std::unordered_map<GlobalNamespace::BeatmapEventData *, std::shared_ptr<ChromaEventData>> EventMapType;
        inline static EventMapType ChromaEventDatas;

        static void deserialize(GlobalNamespace::IReadonlyBeatmapData *beatmapData);
    };



}