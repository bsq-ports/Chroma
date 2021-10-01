#pragma once

#include "sombrero/shared/ColorUtils.hpp"

#include "tracks/shared/Animation/PointDefinition.h"

namespace Chroma {

    class AnimationHelper {
    public:
        static void GetColorOffset(std::optional<PointDefinition*> const& localColor, std::vector<Track*> const& track, float const time, std::optional<Sombrero::FastColor>& color);

    };

}
