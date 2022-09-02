#pragma once

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include "tracks/shared/Animation/Easings.h"

#include <unordered_map>

#include "UnityEngine/Color.hpp"
#include "GlobalNamespace/BasicBeatmapEventData.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"

#include "sombrero/shared/ColorUtils.hpp"

#include "lighting/LerpType.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"

namespace Chroma {
    class ChromaEventData {
    private:
        ChromaEventData(const ChromaEventData&) = default;
        friend class std::unordered_map<GlobalNamespace::BasicBeatmapEventData *, ChromaEventData>;
        friend class std::pair<GlobalNamespace::BasicBeatmapEventData *const, Chroma::ChromaEventData>;

    public:
        ChromaEventData() = default;

        std::optional<Functions> Easing;

        std::optional<LerpType> LerpType;

        std::optional<std::vector<int>> LightID;

        std::optional<std::vector<int>> PropID;

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

        std::unordered_map<int, std::pair<GlobalNamespace::BasicBeatmapEventData*, ChromaEventData*>> NextSameTypeEvent;
    };

    class ChromaEventDataManager {
    public:
        using EventMapType = std::unordered_map<GlobalNamespace::BasicBeatmapEventData *, ChromaEventData>;
        inline static EventMapType ChromaEventDatas;

        static void deserialize(CustomJSONData::CustomBeatmapData *beatmapData);
    };



}