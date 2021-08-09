#include "lighting/environment_enhancements/GameObjectTrackController.hpp"
#include "Chroma.hpp"

#include "lighting/environment_enhancements/EnvironmentEnhancementManager.hpp"
#include "lighting/environment_enhancements/ParametricBoxControllerParameters.hpp"

#include "tracks/shared/Animation/Animation.h"

DEFINE_TYPE(Chroma, GameObjectTrackController)

template<typename T>
static std::optional<T> getPropertyNullable(std::optional<PropertyValue> prop) {
    if (!prop) return std::nullopt;

    // TODO: Left handed

    //    float linear;
    //    NEVector::Vector3 vector3;
    //    NEVector::Vector4 vector4;
    //    NEVector::Quaternion quaternion;

    if constexpr(std::is_same<T, float>::value) {
        return prop.value().linear;
    } else if constexpr(std::is_same<T, NEVector::Vector3>::value) {
        return prop.value().vector3;
    } else if constexpr(std::is_same<T, NEVector::Vector4>::value) {
        return prop.value().vector4;
    } else if constexpr(std::is_same<T, NEVector::Quaternion>::value) {
        return prop.value().quaternion;
    }

    return std::nullopt;
}

static NEVector::Quaternion QuatInverse(const NEVector::Quaternion &a) {
        NEVector::Quaternion conj = {-a.x, -a.y, -a.z, a.w};
         float norm2 = NEVector::Quaternion::Dot(a, a);
         return {conj.x / norm2, conj.y / norm2, conj.z / norm2,
                           conj.w / norm2};
}

void Chroma::GameObjectTrackController::Update() {
    const auto& properties = _track->properties;
    auto rotation = getPropertyNullable<NEVector::Quaternion>(properties.rotation.value);
    auto localRotation = getPropertyNullable<NEVector::Quaternion>(properties.localRotation.value);
    auto position = getPropertyNullable<NEVector::Vector3>(properties.position.value);
    std::optional<NEVector::Vector3> localPosition = std::nullopt; // TODO: getPropertyNullable<NEVector::Vector3>(properties.localPosition.value);
    auto scale = getPropertyNullable<NEVector::Vector3>(properties.scale.value);

    auto transform = get_transform();

    auto transformParent = transform->get_parent();

    if (rotation && transform->get_rotation() != rotation.value())
    {
        // Delegate positioning the object to TrackLaneRing
        NEVector::Quaternion finalOffset;

        if (transformParent != nullptr)
        {
            finalOffset = QuatInverse(transformParent->get_rotation()) * rotation.value();
        }
        else
        {
            finalOffset = rotation.value();
        }

        if (_trackLaneRing)
        {
            EnvironmentEnhancementManager::RingRotationOffsets[_trackLaneRing.value()] = finalOffset;
        }
        else if (_beatmapObjectsAvoidance)
        {
            EnvironmentEnhancementManager::AvoidanceRotation[_beatmapObjectsAvoidance.value()] = finalOffset;
        }
        else
        {
            transform->set_rotation(rotation.value());
        }
    }

    if (localRotation && transform->get_localRotation() != localRotation.value())
    {
        if (_trackLaneRing)
        {
            EnvironmentEnhancementManager::RingRotationOffsets[_trackLaneRing.value()] = localRotation.value();
        }
        else if (_beatmapObjectsAvoidance)
        {
            EnvironmentEnhancementManager::AvoidanceRotation[_beatmapObjectsAvoidance.value()] = localRotation.value();
        }
        else
        {
            transform->set_rotation(localRotation.value());
        }
    }

    if (position && transform->get_position() != (position.value() * _noteLinesDistance))
    {
        NEVector::Vector3 positionValue = position.value() * _noteLinesDistance;
        NEVector::Vector3 finalOffset;
        if (transformParent)
        {
            finalOffset = transformParent->InverseTransformPoint(positionValue);
        }
        else
        {
            finalOffset = positionValue;
        }

        if (_trackLaneRing)
        {
            _trackLaneRing.value()->positionOffset = finalOffset;
        }
        else if (_beatmapObjectsAvoidance)
        {
            EnvironmentEnhancementManager::AvoidancePosition[_beatmapObjectsAvoidance.value()] = finalOffset;
        }
        else
        {
            transform->set_position(positionValue);
        }
    }

    auto localPositionTrans = transform->get_localPosition();

    if (localPosition && localPositionTrans != localPosition.value())
    {
        NEVector::Vector3 localPositionValue = localPosition.value() * _noteLinesDistance;
        if (_trackLaneRing)
        {
            _trackLaneRing.value()->positionOffset = localPosition.value();
        }
        else if (_beatmapObjectsAvoidance)
        {
            EnvironmentEnhancementManager::AvoidancePosition[_beatmapObjectsAvoidance.value()] = localPositionValue;
        }
        else
        {
            transform->set_localPosition(localPositionValue);
        }
    }

    auto localScale = transform->get_localScale();

    if (scale && localScale != scale.value())
    {
        transform->set_localScale(scale.value());
    }

    // Handle ParametricBoxController
    if (_parametricBoxController)
    {
        if (position || localPosition)
        {
            ParametricBoxControllerParameters::SetTransformPosition(_parametricBoxController.value(), localPositionTrans);
        }

        if (scale)
        {
            ParametricBoxControllerParameters::SetTransformScale(_parametricBoxController.value(), localScale);
        }
    }
}

void Chroma::GameObjectTrackController::Init(Track *track, float noteLinesDistance,
                                             std::optional<GlobalNamespace::TrackLaneRing *> trackLaneRing,
                                             std::optional<GlobalNamespace::ParametricBoxController *> parametricBoxController,
                                             std::optional<GlobalNamespace::BeatmapObjectsAvoidance *> beatmapObjectsAvoidance) {
    _track = track;
    _noteLinesDistance = noteLinesDistance;
    _trackLaneRing = trackLaneRing;
    _parametricBoxController = parametricBoxController;
    _beatmapObjectsAvoidance = beatmapObjectsAvoidance;
}

void Chroma::GameObjectTrackController::HandleTrackData(UnityEngine::GameObject *gameObject,
                                                        CustomJSONData::JSONWrapper* gameObjectData,
                                                        CustomJSONData::CustomBeatmapData * beatmapData,
                                                        float noteLinesDistance,
                                                        std::optional<GlobalNamespace::TrackLaneRing *> trackLaneRing,
                                                        std::optional<GlobalNamespace::ParametricBoxController *> parametricBoxController,
                                                        std::optional<GlobalNamespace::BeatmapObjectsAvoidance *> beatmapObjectsAvoidance) {
    auto track = TracksAD::getAD(gameObjectData).track;
    if (track)
    {
        GameObjectTrackController* trackController = gameObject->AddComponent<GameObjectTrackController*>();
        trackController->Init(track, noteLinesDistance, trackLaneRing, parametricBoxController, beatmapObjectsAvoidance);
    }
}
