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
        std::vector<Track*> Tracks;
        std::optional<PointDefinition*> LocalPathColor;

        // note
        std::optional<bool> DisableSpawnEffect;

        ChromaObjectData() = default;
    };

    class ChromaObjectDataManager {
    public:
        using ChromaObjectDataType = std::unordered_map<GlobalNamespace::BeatmapObjectData *, ChromaObjectData>;
        inline static ChromaObjectDataType ChromaObjectDatas;

        static void deserialize(GlobalNamespace::IReadonlyBeatmapData *beatmapData);
    };

}