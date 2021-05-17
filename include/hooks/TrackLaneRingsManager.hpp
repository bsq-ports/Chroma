#include <vector>

#include "GlobalNamespace/TrackLaneRingsManager.hpp"

namespace Chroma {
    class TrackLaneRingsManagerHolder {
    public:
        static std::vector<GlobalNamespace::TrackLaneRingsManager *> RingManagers;
    };
}