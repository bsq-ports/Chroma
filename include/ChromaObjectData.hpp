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
class CustomNoteData;
class CustomObstacleData;
class CustomSliderData;
}

// Third-party includes
#include "tracks/shared/Animation/Track.h"
#include "tracks/shared/Animation/PointDefinition.h"
#include "sombrero/shared/ColorUtils.hpp"

#include "custom-json-data/shared/JSONWrapper.h"

namespace Chroma {

class ChromaObjectData {
public:
  std::optional<Sombrero::FastColor> Color;
  std::span<TrackW const> Tracks; // probably a bad idea, this could be freed.
  std::optional<PointDefinitionW> LocalPathColor;

  // note
  std::optional<bool> DisableDebris;
  std::optional<bool> SpawnEffect;

  ChromaObjectData() = default;
  ChromaObjectData(ChromaObjectData&&) = default;

  [[deprecated("Copy invoked")]]
  ChromaObjectData(ChromaObjectData const&) = default;

  ChromaObjectData& operator=(ChromaObjectData&&) noexcept = default;
  ChromaObjectData& operator=(ChromaObjectData const&) = default;
};

class ChromaObjectDataManager {
public:
  static void deserialize(CustomJSONData::CustomBeatmapData* beatmapData);
};

static ChromaObjectData& getObjectAD(CustomJSONData::JSONWrapper* customData) {
  auto& ad = customData->associatedData['C'];
  if (!ad.has_value()) ad = std::make_any<Chroma::ChromaObjectData>();
  return std::any_cast<Chroma::ChromaObjectData&>(ad);
}

// defined in ChromaObjectData.cpp to avoid include bloat
ChromaObjectData* getObjectAD(GlobalNamespace::BeatmapObjectData* obj);

} // namespace Chroma