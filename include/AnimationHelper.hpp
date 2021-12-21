#pragma once

#include "Chroma.hpp"

#include "sombrero/shared/ColorUtils.hpp"

#include "tracks/shared/Animation/PointDefinition.h"
#include "tracks/shared/Vector.h"
#include "tracks/shared/Animation/Track.h"
#include "tracks/shared/Animation/Animation.h"

namespace Chroma {

    namespace AnimationHelper {
        static std::optional<NEVector::Vector4> TryGetVector4PathProperty(PathProperty &pathProp, float const time) {
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

        static std::optional<NEVector::Vector4> MultiTrackGetPathColor(std::vector<Track *> const &tracks,
                                                                       std::function<std::optional<NEVector::Vector4>(
                                                                               Track *)> const &vectorExpression) {
            bool valid = false;
            NEVector::Vector4 total = NEVector::Vector4{1, 1, 1, 1};

            for (auto &track: tracks) {
                auto result = vectorExpression(track);

                if (result) {
                    total = Vector4Mult(result.value(), total);
                    valid = true;
                }
            }

            return valid ? std::make_optional(total) : std::nullopt;
        }

        static std::optional<Sombrero::FastColor> GetColorOffset(std::optional<PointDefinition *> const &localColor, std::vector<Track *> const &tracks,
                       float const time) {
            std::optional<NEVector::Vector4> pathColor;

            if (localColor) {
                pathColor = localColor.value()->InterpolateVector4(time);
            } else if (tracks.empty()) {
                // Early return because no color will be given
                return std::nullopt;
            }

            std::optional<NEVector::Vector4> colorVector;


            if (!tracks.empty()) {
                if (tracks.size() > 1) {

                    if (!pathColor) {
                        pathColor = MultiTrackGetPathColor(tracks, [&time](Track *track) {
                            auto &colorPathProp = track->pathProperties.color;
                            return TryGetVector4PathProperty(colorPathProp, time);
                        });
                    }

                    auto trackColor = MultiTrackGetPathColor(tracks, [](Track *track) {
                        auto &colorProp = track->properties.color;

                        if (colorProp.value) {
                            return std::make_optional(colorProp.value.value().vector4);
                        }
                        return (std::optional<NEVector::Vector4>) std::nullopt;
                    });

                    colorVector = MultVector4Nullables(trackColor, pathColor);


                } else {
                    Track *trackVal = tracks.front();


                    if (!pathColor) {
                        auto &colorPathProp = trackVal->pathProperties.color;
                        pathColor = TryGetVector4PathProperty(colorPathProp, time);
                    }

                    std::optional<NEVector::Vector4> trackColor;
                    auto const &colorProp = trackVal->properties.color;
                    if (colorProp.value) {
                        trackColor = colorProp.value.value().vector4;
                    }

                    colorVector = MultVector4Nullables(trackColor, pathColor);
                }
            } else {
                colorVector = pathColor;
            }

            if (colorVector) {
                return Sombrero::FastColor(colorVector->x, colorVector->y, colorVector->z, colorVector->w);
            } else {
                return std::nullopt;
            }
        }

    };

}
