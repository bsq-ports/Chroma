#include "AnimationHelper.hpp"
#include "Chroma.hpp"

#include "tracks/shared/Vector.h"
#include "tracks/shared/Animation/Track.h"
#include "tracks/shared/Animation/Animation.h"

std::optional<NEVector::Vector4> TryGetVector4PathProperty(PathProperty &pathProp, float const &time) {
    if (pathProp.value) {
        return pathProp.value.value().InterpolateVector4(time);
    }

    return std::nullopt;
}

// TODO: Sombrero!
static NEVector::Vector4 Vector4Mult(NEVector::Vector4 const &a, NEVector::Vector4 const &b) {
    return {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
}

static std::optional<NEVector::Vector4> MultVector4Nullables(std::optional<NEVector::Vector4> const &vectorOne,
                                                             std::optional<NEVector::Vector4> const &vectorTwo) {
    if (vectorOne) {
        if (vectorTwo) {
            return Vector4Mult(vectorOne.value(), vectorTwo.value());
        } else {
            return vectorOne;
        }
    } else if (vectorTwo) {
        return vectorTwo;
    }

    return std::nullopt;
}

inline static std::string VectorStr(NEVector::Vector4 const &color) {
    return "r: " + std::to_string(color.x) + ", g: " + std::to_string(color.y) + ", b:" + std::to_string(color.z) +
           ", a:" + std::to_string(color.w);
}

void Chroma::AnimationHelper::GetColorOffset(std::optional<PointDefinition *> const &localColor,
                                             std::optional<Track *> const &track,
                                             float const &time, std::optional<Sombrero::FastColor> &color) {
    std::optional<NEVector::Vector4> pathColor;

    if (localColor) {
//        getLogger().debug("Local color!");
        pathColor = localColor.value()->InterpolateVector4(time);
    } else if (!track) {
        // Early return because no color will be given
        color = std::nullopt;
        return;
    }

    // todo: multiple tracks

    std::optional<NEVector::Vector4> colorVector;
    std::optional<NEVector::Vector4> trackColor;

    if (track) {
        Track *trackVal = *track;

        auto &colorPathProp = trackVal->pathProperties.color;

        if (!pathColor)
            pathColor = TryGetVector4PathProperty(colorPathProp, time);


        auto &colorProp = trackVal->properties.color;

        if (colorProp.value) {
            trackColor = colorProp.value.value().vector4;
        }

        colorVector = MultVector4Nullables(trackColor, pathColor);
    } else {
        colorVector = pathColor;
    }

    if (colorVector) {
        NEVector::Vector4 vectorValue(colorVector.value());
        color = Sombrero::FastColor(vectorValue.x, vectorValue.y, vectorValue.z, vectorValue.w);
    } else {
        color = std::nullopt;
    }
}
