#include "lighting/environment_enhancements/GameObjectTrackController.hpp"
#include "Chroma.hpp"
#include "utils/ChromaUtils.hpp"

#include "lighting/environment_enhancements/EnvironmentEnhancementManager.hpp"
#include "lighting/environment_enhancements/ParametricBoxControllerParameters.hpp"

#include "tracks/shared/Animation/Animation.h"

#include "sombrero/shared/Vector3Utils.hpp"
#include "sombrero/shared/QuaternionUtils.hpp"

DEFINE_TYPE(Chroma, GameObjectTrackController)

using namespace Chroma;

template<typename T>
static constexpr std::optional<T> getPropertyNullable(Track* track, const std::optional<PropertyValue>& prop) {
    auto ret = Animation::getPropertyNullable<T>(track, prop);

    if (GameObjectTrackController::LeftHanded) {
        if constexpr(std::is_same_v<T, NEVector::Vector3>) {
            return Animation::MirrorVectorNullable(ret);
        }

        if constexpr(std::is_same_v<T, NEVector::Quaternion>) {
            return Animation::MirrorQuaternionNullable(ret);
        }
    }

    return ret;
}

//static NEVector::Quaternion QuatInverse(const NEVector::Quaternion &a) {
//        NEVector::Quaternion conj = {-a.x, -a.y, -a.z, a.w};
//         float norm2 = NEVector::Quaternion::Dot(a, a);
//         return {conj.x / norm2, conj.y / norm2, conj.z / norm2,
//                           conj.w / norm2};
//}



void Chroma::GameObjectTrackController::ClearData() {
    _dataMap.clear();
    _dataMap = {};
    nextId = 0;
}

void Chroma::GameObjectTrackController::Update() {
    if (!data) {
        auto it = _dataMap.find(id);

        if (it != _dataMap.end()) {
            data = &it->second;
        }
    }

    if (!data){
        getLogger().error("Data is null! Should remove component or just early return? %p %s", this, static_cast<std::string>(get_gameObject()->get_name()).c_str());
        Destroy(this);
        return;
    }
    const auto _noteLinesDistance = data->_noteLinesDistance;
    const auto _track = data->_track;
    // nullable
    const auto _trackLaneRing = data->_trackLaneRing;
    const auto _parametricBoxController = data->_parametricBoxController;
    const auto _beatmapObjectsAvoidance = data->_beatmapObjectsAvoidance;


    if (!_track){
        getLogger().error("Track is null! Should remove component or just early return? %p %s", this, static_cast<std::string>(get_gameObject()->get_name()).c_str());
        Destroy(this);
        return;
    }
    const auto& properties = _track->properties;
    const auto rotation = getPropertyNullable<NEVector::Quaternion>(_track, properties.rotation.value);
    const auto localRotation = getPropertyNullable<NEVector::Quaternion>(_track, properties.localRotation.value);
    const auto position = getPropertyNullable<NEVector::Vector3>(_track, properties.position.value);
    const auto localPosition = getPropertyNullable<NEVector::Vector3>(_track, properties.localPosition.value);
    const auto scale = getPropertyNullable<NEVector::Vector3>(_track, properties.scale.value);

    auto transform = get_transform();

    auto transformParent = transform->get_parent();

    bool updateParametricBox = false;

    if (rotation)
    {
        // Delegate positioning the object to TrackLaneRing
        NEVector::Quaternion finalOffset;

        if (transformParent)
        {
            finalOffset = NEVector::Quaternion(UnityEngine::Quaternion::Inverse(transformParent->get_rotation())) * rotation.value();
        }
        else
        {
            finalOffset = rotation.value();
        }

        if (_trackLaneRing)
        {
            EnvironmentEnhancementManager::RingRotationOffsets[_trackLaneRing] = finalOffset;
        }
        else if (_beatmapObjectsAvoidance)
        {
            EnvironmentEnhancementManager::AvoidanceRotation[_beatmapObjectsAvoidance] = finalOffset;
        }
        else
        {
            transform->set_rotation(rotation.value());
        }
    }

    if (localRotation)
    {
        if (_trackLaneRing)
        {
            EnvironmentEnhancementManager::RingRotationOffsets[_trackLaneRing] = localRotation.value();
        }
        else if (_beatmapObjectsAvoidance)
        {
            EnvironmentEnhancementManager::AvoidanceRotation[_beatmapObjectsAvoidance] = localRotation.value();
        }
        else
        {
            transform->set_localRotation(localRotation.value());
        }
    }


    if (position)
    {
        Sombrero::FastVector3 positionValue = position.value() * _noteLinesDistance;
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
            _trackLaneRing->positionOffset = finalOffset;
        }
        else if (_beatmapObjectsAvoidance)
        {
            EnvironmentEnhancementManager::AvoidancePosition[_beatmapObjectsAvoidance] = finalOffset;
        }
        else
        {
            transform->set_position(positionValue);
            updateParametricBox = true;
        }
    }

    if (localPosition)
    {
        NEVector::Vector3 localPositionValue = localPosition.value() * _noteLinesDistance;
        if (_trackLaneRing)
        {
            _trackLaneRing->positionOffset = localPositionValue;
        }
        else if (_beatmapObjectsAvoidance)
        {
            EnvironmentEnhancementManager::AvoidancePosition[_beatmapObjectsAvoidance] = localPositionValue;
        }
        else
        {
            transform->set_localPosition(localPositionValue);
            updateParametricBox = true;
        }
    }

    if (scale)
    {
        transform->set_localScale(scale.value());
        updateParametricBox = true;
    }

    // Handle ParametricBoxController
    if (updateParametricBox && _parametricBoxController)
    {
        ParametricBoxControllerParameters::SetTransformPosition(_parametricBoxController, transform->get_localPosition());
        ParametricBoxControllerParameters::SetTransformScale(_parametricBoxController, transform->get_localScale());
    }
}

void Chroma::GameObjectTrackController::Init(Track *track, float noteLinesDistance,
                                             GlobalNamespace::TrackLaneRing * trackLaneRing,
                                             GlobalNamespace::ParametricBoxController * parametricBoxController,
                                             GlobalNamespace::BeatmapObjectsAvoidance * beatmapObjectsAvoidance) {
    CRASH_UNLESS(track);
    this->data = &_dataMap.try_emplace(nextId, track, trackLaneRing, parametricBoxController, beatmapObjectsAvoidance, noteLinesDistance).first->second;
    nextId++;
}

void Chroma::GameObjectTrackController::HandleTrackData(UnityEngine::GameObject *gameObject,
                                                        std::optional<Track*> track,
                                                        float noteLinesDistance,
                                                        GlobalNamespace::TrackLaneRing * trackLaneRing,
                                                        GlobalNamespace::ParametricBoxController * parametricBoxController,
                                                        GlobalNamespace::BeatmapObjectsAvoidance * beatmapObjectsAvoidance) {
    GameObjectTrackController* existingTrackController = gameObject->GetComponent<GameObjectTrackController*>();
    if (existingTrackController)
    {
        Destroy(existingTrackController);
    }

    if (track)
    {
        GameObjectTrackController* trackController = gameObject->AddComponent<GameObjectTrackController*>();
        trackController->Init(track.value(), noteLinesDistance, trackLaneRing, parametricBoxController, beatmapObjectsAvoidance);

        track.value()->AddGameObject(gameObject);
    }
}
