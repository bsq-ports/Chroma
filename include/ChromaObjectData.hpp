#pragma once

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include <unordered_map>

#include "UnityEngine/Color.hpp"
#include "GlobalNamespace/BeatmapObjectData.hpp"
#include "GlobalNamespace/BeatmapEventData.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"

#include "tracks/shared/Animation/Track.h"
#include "tracks/shared/Animation/PointDefinition.h"

#include "sombrero/shared/ColorUtils.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"

namespace Chroma {

    enum class ObjectDataType {
        Note,
        Object,
        Noodle
    };

    class ChromaObjectData {
    private:
        ChromaObjectData(const ChromaObjectData&) = default;
        friend class std::unordered_map<GlobalNamespace::BeatmapObjectData *, ChromaObjectData>;
        friend class std::pair<GlobalNamespace::BeatmapObjectData *const, Chroma::ChromaObjectData>;

    public:
        std::optional<Sombrero::FastColor> Color;
        std::span<Track*> Tracks; // probably a bad idea
        std::optional<PointDefinition*> LocalPathColor;

        // note
        std::optional<bool> DisableSpawnEffect;

        ChromaObjectData() = default;
        ChromaObjectData(ChromaObjectData&&) = default;
    };

    class ChromaObjectDataManager {
    public:
        using ChromaObjectDataType = std::unordered_map<GlobalNamespace::BeatmapObjectData *, ChromaObjectData>;
        inline static ChromaObjectDataType ChromaObjectDatas;

        static void deserialize(CustomJSONData::CustomBeatmapData *beatmapData);
    };

}