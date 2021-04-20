#pragma once

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include "utils/Easing.hpp"

#include <unordered_map>

#include "UnityEngine/Color.hpp"
#include "GlobalNamespace/BeatmapEventData.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"

using namespace ChromaUtils;

namespace Chroma {

    enum class EventDataType {
        LIGHT,
        RING,
        LASER_SPEED,
        RING_STEP
    };

    class ChromaEventData {
    public:
        const EventDataType dataType;

        explicit ChromaEventData(EventDataType dataType1) : dataType(dataType1) {};
    };

    class ChromaEventDataManager {
    public:
        typedef std::unordered_map<GlobalNamespace::BeatmapEventData *, std::shared_ptr<ChromaEventData>> EventMapType;
        inline static EventMapType ChromaEventDatas;

        static void deserialize(GlobalNamespace::IReadonlyBeatmapData *beatmapData);
    };


    class ChromaLightEventData : public ChromaEventData {
    public:
        ChromaLightEventData() : ChromaEventData(EventDataType::LIGHT) {};

        std::optional<std::reference_wrapper<rapidjson::Value>> LightID;

        std::optional<std::reference_wrapper<rapidjson::Value>> PropID;

        std::optional<UnityEngine::Color> ColorData;

        struct GradientObjectData {
            float Duration;

            UnityEngine::Color StartColor;

            UnityEngine::Color EndColor;

            Functions Easing;
        };

        std::optional<GradientObjectData> GradientObject;
    };

    class ChromaRingRotationEventData : public ChromaEventData {
    public:
        ChromaRingRotationEventData() : ChromaEventData(EventDataType::RING) {};
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
    };

    class ChromaLaserSpeedEventData : public ChromaEventData {
    public:
        ChromaLaserSpeedEventData() : ChromaEventData(EventDataType::LASER_SPEED) {};

        bool LockPosition;

        float PreciseSpeed;

        int Direction;
    };

    class ChromaRingStepEventData  : public ChromaEventData  {
    public:
        ChromaRingStepEventData () : ChromaEventData(EventDataType::RING_STEP) {};


        std::optional<float> Step;
    };

}