#pragma once

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include "tracks/shared/Animation/Easings.h"

#include <unordered_map>

#include "sombrero/shared/ColorUtils.hpp"

#include "lighting/LerpType.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"

namespace GlobalNamespace {
class BasicBeatmapEventData;
class BeatmapObjectSpawnController;
};

namespace Chroma {
class ChromaEventData {
public:
  ChromaEventData() = default;

  [[deprecated("Copy invoked")]]
  ChromaEventData(ChromaEventData const&) = default;
  ChromaEventData(ChromaEventData&&) = default;

  ChromaEventData& operator=(ChromaEventData&&) noexcept = default;
  ChromaEventData& operator=(ChromaEventData const&) = default;

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

  std::unordered_map<int, std::pair<GlobalNamespace::BeatmapEventData*, ChromaEventData*>> NextSameTypeEvent;
};

class ChromaEventDataManager {
public:
  static void deserialize(CustomJSONData::CustomBeatmapData* beatmapData);
};

static ChromaEventData& getLightAD(CustomJSONData::JSONWrapper* customData) {
  auto& ad = customData->associatedData['C'];
  if (!ad.has_value()) ad = std::make_any<Chroma::ChromaEventData>();
  return std::any_cast<Chroma::ChromaEventData&>(ad);
}

} // namespace Chroma