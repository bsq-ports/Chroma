#pragma once

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include <unordered_map>

#include "UnityEngine/Color.hpp"
#include "GlobalNamespace/BeatmapObjectData.hpp"
#include "GlobalNamespace/BeatmapEventData.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"

namespace Chroma {

    enum class ObjectDataType {
        Note,
        Object,
        Noodle
    };

    class ChromaObjectData {
    public:
        const ObjectDataType dataType;
        std::optional<UnityEngine::Color> Color;

        ChromaObjectData() : dataType(ObjectDataType::Object){};

        explicit ChromaObjectData(ObjectDataType dataType1) : dataType(dataType1) {};
    };

    class ChromaObjectDataManager {
    public:
        typedef std::unordered_map<GlobalNamespace::BeatmapObjectData *, std::shared_ptr<ChromaObjectData>> ChromaObjectDataType;
        inline static ChromaObjectDataType ChromaObjectDatas;

        static void deserialize(GlobalNamespace::IReadonlyBeatmapData *beatmapData);
    };

    class ChromaNoteData : public ChromaObjectData
    {
    public:
        ChromaNoteData() : ChromaObjectData(ObjectDataType::Note) {};

        std::optional<bool> DisableSpawnEffect;
    };

//    class ChromaNoodleData
//    {
//        Track Track;
//
//        internal PointDefinition LocalPathColor { get; set; }
//    }



}