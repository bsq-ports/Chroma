#pragma once

#include <optional>
#include <span>
#include <unordered_map>

// Forward declarations
namespace GlobalNamespace {
class BeatmapObjectData;
}

namespace CustomJSONData {
class CustomBeatmapData;
}

// Third-party includes
#include "tracks/shared/Animation/Track.h"
#include "tracks/shared/Animation/PointDefinition.h"
#include "sombrero/shared/ColorUtils.hpp"

namespace Chroma {

class ChromaObjectData {
private:
  ChromaObjectData(ChromaObjectData const&) = default;
  friend class std::unordered_map<GlobalNamespace::BeatmapObjectData*, ChromaObjectData>;
  friend class std::pair<GlobalNamespace::BeatmapObjectData* const, Chroma::ChromaObjectData>;

public:
  std::optional<Sombrero::FastColor> Color;
  std::span<TrackW const> Tracks; // probably a bad idea, this could be freed.
  std::optional<PointDefinitionW> LocalPathColor;

  // note
  std::optional<bool> SpawnEffect;

  ChromaObjectData() = default;
  ChromaObjectData(ChromaObjectData&&) = default;
};

class ChromaObjectDataManager {
public:
  using ChromaObjectDataType = std::unordered_map<GlobalNamespace::BeatmapObjectData*, ChromaObjectData>;
  inline static ChromaObjectDataType ChromaObjectDatas;

  static void deserialize(CustomJSONData::CustomBeatmapData* beatmapData);
};

} // namespace Chroma