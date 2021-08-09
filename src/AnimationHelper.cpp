#include "AnimationHelper.hpp"
#include "Chroma.hpp"

#include "tracks/shared/Vector.h"
#include "tracks/shared/Animation/Track.h"
#include "tracks/shared/Animation/Animation.h"

std::optional<PointDefinitionInterpolation> GetPathInterpolation(Track *track, std::string_view name, PropertyType type) {
    return track ? track->pathProperties.FindProperty(name)->value : std::nullopt;
}

std::optional<NEVector::Vector4> TryGetVector4PathProperty(Track *track, std::string_view name, float time) {
    std::optional<PointDefinitionInterpolation> pointDataInterpolation = GetPathInterpolation(track, name, PropertyType::vector4);
    if (pointDataInterpolation) {
        return pointDataInterpolation->InterpolateVector4(time);
    }
    return std::nullopt;
}

// TODO: Sombrero!
static NEVector::Vector4 Vector4Mult(NEVector::Vector4 const& a,NEVector::Vector4 const& b) {
    return {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
}

void Chroma::AnimationHelper::GetColorOffset(std::optional<PointDefinition *> localColor, std::optional<Track *> track,
                                             float time, std::optional<Sombrero::FastColor> &color) {
    std::optional<NEVector::Vector4> pathColor;

    if (track) {
        if (localColor) {
            pathColor = localColor.value()->InterpolateVector4(time);
        } else {
            pathColor = TryGetVector4PathProperty(track.value(), Chroma::COLOR, time);
        }
    }

    std::optional<NEVector::Vector4> colorVector;
    if (track && pathColor) {
        auto colorProp = track.value()->properties.color;
        if (colorProp.value) {
            colorVector = Vector4Mult(colorProp.value.value().vector4, pathColor.value());
        }
    }

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
