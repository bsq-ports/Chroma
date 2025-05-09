#pragma once

#include <unordered_map>
#include <string_view>
#include <variant>
#include <vector>
// Forward declarations
namespace CustomJSONData {
class CustomEventData;
}

#include "tracks/shared/AssociatedData.h"
#include "tracks/shared/Animation/Track.h"
#include "custom-json-data/shared/CustomEventData.h"

// Custom events, not Beatmap Events
namespace ChromaEvents {
struct AssignBloomFogTrack {
  explicit AssignBloomFogTrack(TrackW track) : track(track) {}
  TrackW track;
};

struct AnimateComponentEventData {
  struct ComponentData {
    ComponentData(std::string_view propertyName, PointDefinitionW prop) : propertyName(propertyName), prop(prop) {}
    std::string_view propertyName;
    PointDefinitionW prop;
  };

  AnimateComponentEventData(AnimateComponentEventData&&) = default;
  AnimateComponentEventData(AnimateComponentEventData const&) = delete;
  ~AnimateComponentEventData() = default;
  AnimateComponentEventData() = default;
  AnimateComponentEventData(float duration, Functions easing, TracksAD::TracksVector track,
                            std::unordered_map<std::string_view, std::vector<ComponentData>> coroutineInfos)
      : duration(duration), easing(easing), track(std::move(track)), coroutineInfos(std::move(coroutineInfos)) {}

  float duration;
  Functions easing;
  TracksAD::TracksVector track;

  // COMPONENT NAME -> CoroutineInfo
  std::unordered_map<std::string_view, std::vector<ComponentData>> coroutineInfos;
};

struct CustomEventAssociatedData {
  std::variant<AssignBloomFogTrack, AnimateComponentEventData, void*> data;
  bool parsed = false;

  CustomEventAssociatedData() : data(nullptr) {};
};

static std::unordered_map<CustomJSONData::CustomEventData const*, CustomEventAssociatedData> eventDataMap;

static CustomEventAssociatedData& getEventAD(CustomJSONData::CustomEventData const* customData) {
  return eventDataMap[customData];
}

void deserialize(CustomJSONData::CustomBeatmapData* readOnlyBeatmap);

void parseEventData(TracksAD::BeatmapAssociatedData& beatmapAD, CustomJSONData::CustomEventData const* customEventData,
                    bool v2);

void AddEventCallbacks();
} // namespace ChromaEvents
