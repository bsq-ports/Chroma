#pragma once

#include "Chroma.hpp"

#include "sombrero/shared/ColorUtils.hpp"

#include "tracks/shared/Animation/PointDefinition.h"
#include "tracks/shared/Vector.h"
#include "tracks/shared/Animation/Track.h"
#include "tracks/shared/Animation/Animation.h"

namespace Chroma {

namespace AnimationHelper {

// TODO: Sombrero!
static NEVector::Vector4 Vector4Mult(NEVector::Vector4 const& a, NEVector::Vector4 const& b) {
  return { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w };
}

static std::optional<NEVector::Vector4> MultVector4Nullables(std::optional<NEVector::Vector4> const& vectorOne,
                                                             std::optional<NEVector::Vector4> const& vectorTwo) {
  if (vectorOne) {
    if (vectorTwo) {
      return Vector4Mult(vectorOne.value(), vectorTwo.value());
    }
    return vectorOne;
  }

  if (vectorTwo) {
    return vectorTwo;
  }

  return std::nullopt;
}

template <typename F = std::function<std::optional<NEVector::Vector4>(TrackW)> const&>
static std::optional<NEVector::Vector4> MultiTrackGetPathColor(std::span<TrackW> const& tracks, F vectorExpression) {
  bool valid = false;
  NEVector::Vector4 total = NEVector::Vector4{ 1, 1, 1, 1 };

  for (auto track : tracks) {
    auto result = vectorExpression(track);

    if (result) {
      total = Vector4Mult(result.value(), total);
      valid = true;
    }
  }

  return valid ? std::make_optional(total) : std::nullopt;
}

static std::optional<Sombrero::FastColor> GetColorOffset(std::optional<PointDefinitionW> const& localColor,
                                                         std::span<TrackW const> const& tracksOpt, float const time,
                                                         bool& trackUpdated, Tracks::ffi::BaseProviderContext* context,
                                                         TimeUnit lastCheckedTime = {}) {
  std::optional<NEVector::Vector4> pathColor;

  bool last = false;

  if (localColor) {
    pathColor = localColor.value().InterpolateVector4(time, last);
  } else if (tracksOpt.empty()) {
    // Early return because no color will be given
    return std::nullopt;
  }

  std::optional<NEVector::Vector4> colorVector;

  if (!tracksOpt.empty()) {
    std::span<TrackW const> const& tracks = tracksOpt;
    if (tracks.size() > 1) {

      if (!pathColor) {
        auto pathColors = Animation::getPathPropertiesVec4(tracks, PropertyNames::Color, context, time);

        pathColor = Animation::multiplyVector4s(pathColors);
      }

      auto trackColors = Animation::getPathPropertiesVec4(tracks, PropertyNames::Color, context, time);

      auto trackColor = Animation::multiplyVector4s(trackColors);

      colorVector = MultVector4Nullables(trackColor, pathColor);

    } else {
      TrackW trackVal = tracks.front();

      if (!pathColor) {
        auto colorPathProp = trackVal.GetPathPropertyNamed(PropertyNames::Color);
        pathColor = colorPathProp.InterpolateVec4(time, last, context);
      }

      std::optional<NEVector::Vector4> trackColor;

      auto colorProp = trackVal.GetPropertyNamed(PropertyNames::Color);
      auto color = colorProp.GetVec4(lastCheckedTime);
      if (color) {
        trackUpdated = true;
        trackColor = color.value();
      }

      colorVector = MultVector4Nullables(trackColor, pathColor);
    }
  } else {
    colorVector = pathColor;
  }

  if (colorVector) {
    return Sombrero::FastColor(colorVector->x, colorVector->y, colorVector->z, colorVector->w);
  }
  return std::nullopt;
}

}; // namespace AnimationHelper

} // namespace Chroma
