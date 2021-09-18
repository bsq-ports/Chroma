#pragma once

#include "GlobalNamespace/TrackLaneRingsManager.hpp"

namespace Chroma {

    enum class ComponentType {
        TrackLaneRingsManager
    };

    class IComponentData {
    public:
        virtual ComponentType getComponentType() = 0;
    };

    // TODO: Maybe custom-type?
    class TrackLaneRingsManagerComponentData : public IComponentData
    {
    public:
        SafePtr<GlobalNamespace::TrackLaneRingsManager> OldTrackLaneRingsManager;

        SafePtr<GlobalNamespace::TrackLaneRingsManager> NewTrackLaneRingsManager;

        ComponentType getComponentType() override;
    };
}


