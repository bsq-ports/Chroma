#pragma once

#include "GlobalNamespace/TrackLaneRingsManager.hpp"

namespace Chroma {

enum class ComponentType { TrackLaneRingsManager };

class IComponentData {
public:
  virtual ComponentType getComponentType() = 0;
};

// TODO: Maybe custom-type?
class TrackLaneRingsManagerComponentData : public IComponentData {
public:
  SafePtrUnity<GlobalNamespace::TrackLaneRingsManager> OldTrackLaneRingsManager;

  SafePtrUnity<GlobalNamespace::TrackLaneRingsManager> NewTrackLaneRingsManager;

  ComponentType getComponentType() override;
};
} // namespace Chroma
