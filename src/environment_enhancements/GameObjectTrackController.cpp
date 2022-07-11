#include "environment_enhancements/GameObjectTrackController.hpp"
#include "Chroma.hpp"
#include "utils/ChromaUtils.hpp"

#include "environment_enhancements/EnvironmentEnhancementManager.hpp"
#include "environment_enhancements/ParametricBoxControllerParameters.hpp"

#include "tracks/shared/Animation/Animation.h"

#include "sombrero/shared/Vector3Utils.hpp"
#include "sombrero/shared/QuaternionUtils.hpp"

DEFINE_TYPE(Chroma, GameObjectTrackController)

using namespace Chroma;

template<typename T>
static constexpr std::optional<T> getPropertyNullable(Track* track, const Property& prop, uint32_t lastCheckedTime) {
    if (lastCheckedTime != 0 && prop.lastUpdated != 0 && prop.lastUpdated < lastCheckedTime) return std::nullopt;

    auto ret = Animation::getPropertyNullable<T>(track, prop.value);

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
    nextId = 0;
}

void GameObjectTrackController::Awake() {
//    OnTransformParentChanged();
}

void GameObjectTrackController::OnEnable() {
    OnTransformParentChanged();
}

void GameObjectTrackController::OnTransformParentChanged() {
    origin = get_transform();
    parent = origin->get_parent();
    UpdateData(true);
}

void GameObjectTrackController::Update() {
    UpdateData(false);
}

void Chroma::GameObjectTrackController::UpdateData(bool force) {
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
    const auto _noteLinesDistance = 0.6f; // StaticBeatmapObjectSpawnMovementData.kNoteLinesDistance
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
    if (force) {
        lastCheckedTime = 0;
    }

    const auto& properties = _track->properties;
    const auto rotation = getPropertyNullable<NEVector::Quaternion>(_track, properties.rotation, lastCheckedTime);
    const auto localRotation = getPropertyNullable<NEVector::Quaternion>(_track, properties.localRotation, lastCheckedTime);
    const auto position = getPropertyNullable<NEVector::Vector3>(_track, properties.position, lastCheckedTime);
    const auto localPosition = getPropertyNullable<NEVector::Vector3>(_track, properties.localPosition, lastCheckedTime);
    const auto scale = getPropertyNullable<NEVector::Vector3>(_track, properties.scale, lastCheckedTime);

    auto transform = origin;

    auto transformParent = parent;

    bool updateParametricBox = false;

    static auto Quaternion_Inverse = il2cpp_utils::il2cpp_type_check::FPtrWrapper<&NEVector::Quaternion::Inverse>::get();
    static auto Transform_Position = il2cpp_utils::il2cpp_type_check::FPtrWrapper<&UnityEngine::Transform::set_position>::get();
    static auto Transform_LocalPosition = il2cpp_utils::il2cpp_type_check::FPtrWrapper<&UnityEngine::Transform::set_localPosition>::get();
    static auto Transform_GetLocalPosition = il2cpp_utils::il2cpp_type_check::FPtrWrapper<&UnityEngine::Transform::get_localPosition>::get();
    static auto Transform_Rotation = il2cpp_utils::il2cpp_type_check::FPtrWrapper<&UnityEngine::Transform::set_rotation>::get();
    static auto Transform_GetRotation = il2cpp_utils::il2cpp_type_check::FPtrWrapper<&UnityEngine::Transform::get_rotation>::get();
    static auto Transform_LocalRotation = il2cpp_utils::il2cpp_type_check::FPtrWrapper<&UnityEngine::Transform::set_localRotation>::get();
    static auto Transform_Scale = il2cpp_utils::il2cpp_type_check::FPtrWrapper<&UnityEngine::Transform::set_localScale>::get();
    static auto Transform_GetScale = il2cpp_utils::il2cpp_type_check::FPtrWrapper<&UnityEngine::Transform::get_localScale>::get();
    static auto Transform_InverseTransformPoint = il2cpp_utils::il2cpp_type_check::FPtrWrapper<static_cast<::UnityEngine::Vector3 (UnityEngine::Transform::*)(::UnityEngine::Vector3)>(&UnityEngine::Transform::InverseTransformPoint)>::get();

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
            Transform_LocalRotation(transform, localRotation.value());
        }
    } else if (rotation)
    {
        // Delegate positioning the object to TrackLaneRing
        NEVector::Quaternion finalOffset;

        if (transformParent && (_trackLaneRing || _beatmapObjectsAvoidance))
        {
            finalOffset = NEVector::Quaternion(Quaternion_Inverse(Transform_GetRotation(transformParent))) * rotation.value();
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
            Transform_Rotation(transform, rotation.value());
        }
    }

    if (localPosition)
    {
        NEVector::Vector3 localPositionValue = localPosition.value();
        if (data->v2) {
            localPositionValue *= _noteLinesDistance;
        }

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
            Transform_LocalPosition(transform, localPositionValue);
            updateParametricBox = true;
        }
    } else if (position)
    {
        Sombrero::FastVector3 positionValue = position.value();
        NEVector::Vector3 finalOffset;

        if (data->v2) {
            positionValue *= _noteLinesDistance;
        }

        if (transformParent)
        {
            finalOffset = Transform_InverseTransformPoint(transformParent, positionValue);
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
            Transform_Position(transform, positionValue);
            updateParametricBox = true;
        }
    }

    if (scale)
    {
        Transform_Scale(transform, scale.value());
        updateParametricBox = true;
    }

    // Handle ParametricBoxController
    if (updateParametricBox && _parametricBoxController)
    {
        ParametricBoxControllerParameters::SetTransformPosition(_parametricBoxController, Transform_GetLocalPosition(transform));
        ParametricBoxControllerParameters::SetTransformScale(_parametricBoxController, Transform_GetScale(transform));
    }

    lastCheckedTime = getCurrentTime();
}

void Chroma::GameObjectTrackController::Init(Track *track, float noteLinesDistance,
                                             GlobalNamespace::TrackLaneRing * trackLaneRing,
                                             GlobalNamespace::ParametricBoxController * parametricBoxController,
                                             GlobalNamespace::BeatmapObjectsAvoidance * beatmapObjectsAvoidance, bool v2) {
    CRASH_UNLESS(track);
    this->data = &_dataMap.try_emplace(nextId, track, trackLaneRing, parametricBoxController, beatmapObjectsAvoidance, noteLinesDistance, v2).first->second;
    nextId++;
}

void Chroma::GameObjectTrackController::HandleTrackData(UnityEngine::GameObject *gameObject,
                                                        std::optional<Track*> track,
                                                        float noteLinesDistance,
                                                        GlobalNamespace::TrackLaneRing * trackLaneRing,
                                                        GlobalNamespace::ParametricBoxController * parametricBoxController,
                                                        GlobalNamespace::BeatmapObjectsAvoidance * beatmapObjectsAvoidance, bool v2) {
    auto* existingTrackController = gameObject->GetComponent<GameObjectTrackController*>();
    if (existingTrackController)
    {
        Destroy(existingTrackController);
    }

    if (track)
    {
        auto* trackController = gameObject->AddComponent<GameObjectTrackController*>();
        trackController->Init(track.value(), noteLinesDistance, trackLaneRing, parametricBoxController, beatmapObjectsAvoidance, v2);

        track.value()->AddGameObject(gameObject);
    }
}
