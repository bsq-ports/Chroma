#pragma once

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include "utils/Easing.hpp"

#include <unordered_map>

#include "UnityEngine/Color.hpp"
#include "GlobalNamespace/BeatmapEventData.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"

using namespace ChromaUtils;

namespace Chroma {

    enum class DataType {
        LIGHT,
        RING,
        LASER_SPEED
    };

    class ChromaEventData {
    public:
        const DataType dataType;

        explicit ChromaEventData(DataType dataType1) : dataType(dataType1) {};
    };

    class ChromaEventDataManager {
    public:
        typedef std::unordered_map<GlobalNamespace::BeatmapEventData *, std::shared_ptr<ChromaEventData>> EventMapType;
        inline static EventMapType ChromaEventDatas;

        static void deserialize(GlobalNamespace::IReadonlyBeatmapData *beatmapData);
    };


    class ChromaLightEventData : public ChromaEventData {
    public:
        ChromaLightEventData() : ChromaEventData(DataType::LIGHT) {};

        std::optional<rapidjson::Value*> LightID;

        std::optional<rapidjson::Value*> PropID;

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
        ChromaRingRotationEventData() : ChromaEventData(DataType::RING) {};
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
        ChromaLaserSpeedEventData() : ChromaEventData(DataType::LASER_SPEED) {};

        bool LockPosition;

        float PreciseSpeed;

        int Direction;
    };

}