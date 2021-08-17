#include "AnimationHelper.hpp"
#include "Chroma.hpp"

#include "tracks/shared/Vector.h"
#include "tracks/shared/Animation/Track.h"
#include "tracks/shared/Animation/Animation.h"

std::optional<NEVector::Vector4> TryGetVector4PathProperty(PathProperty& pathProp, float time) {
    if (pathProp.value) {
        return pathProp.value.value().InterpolateVector4(time);
    }

    return std::nullopt;
}

// TODO: Sombrero!
static NEVector::Vector4 Vector4Mult(NEVector::Vector4 const& a,NEVector::Vector4 const& b) {
    return {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
}

static std::optional<NEVector::Vector4> MultVector4Nullables(std::optional<NEVector::Vector4> vectorOne, std::optional<NEVector::Vector4> vectorTwo)
{
    if (vectorOne)
    {
        if (vectorTwo)
        {
            return Vector4Mult(vectorOne.value(), vectorTwo.value());
        }
        else
        {
            return vectorOne;
        }
    }
    else if (vectorTwo)
    {
        return vectorTwo;
    }

    return std::nullopt;
}

void Chroma::AnimationHelper::GetColorOffset(std::optional<PointDefinition *> localColor, std::optional<Track *> track,
                                             float time, std::optional<Sombrero::FastColor> &color) {
    std::optional<NEVector::Vector4> pathColor;
    std::optional<NEVector::Vector4> trackColor;

    if (track) {
        if (localColor) {
            pathColor = localColor.value()->InterpolateVector4(time);
        } else {
            auto colorPathProp = track.value()->pathProperties.color;

            pathColor = TryGetVector4PathProperty(colorPathProp, time);
        }

        auto colorProp = track.value()->properties.color;

        if (colorProp.value) {
            trackColor = colorProp.value.value().vector4;
        }
    }

    std::optional<NEVector::Vector4> colorVector = MultVector4Nullables(trackColor, pathColor);

    if (colorVector)
    {
        NEVector::Vector4 vectorValue = colorVector.value();
        color = Sombrero::FastColor(vectorValue.x, vectorValue.y, vectorValue.z, vectorValue.w);
    }
    else
    {
        color = std::nullopt;
    }
}
