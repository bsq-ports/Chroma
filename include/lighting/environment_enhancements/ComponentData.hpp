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

    class TrackLaneRingsManagerComponentData : public IComponentData
    {
    public:
        GlobalNamespace::TrackLaneRingsManager* OldTrackLaneRingsManager = nullptr;

        GlobalNamespace::TrackLaneRingsManager* NewTrackLaneRingsManager = nullptr;

        ComponentType getComponentType() override;
    };
}


