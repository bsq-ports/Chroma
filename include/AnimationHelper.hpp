#pragma once

#include "sombrero/shared/ColorUtils.hpp"

#include "tracks/shared/Animation/PointDefinition.h"

namespace Chroma {

    class AnimationHelper {
    public:
        static void GetColorOffset(std::optional<PointDefinition*> localColor, std::optional<Track*> track, float time, std::optional<Sombrero::FastColor>& color);

    };

}
