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

template<typename T, typename F>
static std::optional<std::vector<T>> getPropertiesNullable(std::span<Track const*> tracks, F&& propFn, uint32_t lastCheckedTime) {
    if (tracks.empty()) return std::nullopt;

    std::vector<T> props;

    for (auto t : tracks) {
        if (!t) continue;
        auto const& prop = propFn(t->properties);

        if (lastCheckedTime != 0 && prop.lastUpdated != 0 && prop.lastUpdated < lastCheckedTime) continue;

        auto val = Animation::getPropertyNullable<T>(t, prop.value);
        if (val)
            props.template emplace_back(*val);
    }

    if (props.empty()) return std::nullopt;

    return props;
}

template<typename T>
static constexpr std::optional<T> getPropertyNullable(Track const* track, const Property& prop, uint32_t lastCheckedTime) {
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


    if (_track.empty()){
        getLogger().error("Track is null! Should remove component or just early return? %p %s", this, static_cast<std::string>(get_gameObject()->get_name()).c_str());
        Destroy(this);
        return;
    }
    if (force) {
        lastCheckedTime = 0;
    }

    std::optional<NEVector::Quaternion> rotation;
    std::optional<NEVector::Quaternion> localRotation;
    std::optional<NEVector::Vector3> position;
    std::optional<NEVector::Vector3> localPosition;
    std::optional<NEVector::Vector3> scale;

    // I hate this
    auto tracks = std::span<Track const*>(const_cast<Track const **>(&*_track.begin()), _track.size());

    if (tracks.size() == 1) {
        auto track = tracks.front();
        auto properties = track->properties;

        rotation = getPropertyNullable<NEVector::Quaternion>(track, properties.rotation, lastCheckedTime);
        localRotation = getPropertyNullable<NEVector::Quaternion>(track, properties.localRotation, lastCheckedTime);
        position = getPropertyNullable<NEVector::Vector3>(track, properties.position, lastCheckedTime);
        localPosition = getPropertyNullable<NEVector::Vector3>(track, properties.localPosition, lastCheckedTime);
        scale = getPropertyNullable<NEVector::Vector3>(track, properties.scale, lastCheckedTime);

    } else {

#define combine(target, list, op) \
        if (list)                          \
        for (auto const& i : *list) {      \
            if (!target) target = i;       \
            else target = *target op i;\
        }

        auto localRotations = getPropertiesNullable<NEVector::Quaternion>(tracks, [](Properties const& p) {return p.localRotation;}, lastCheckedTime);
        auto rotations = getPropertiesNullable<NEVector::Quaternion>(tracks, [](Properties const& p) {return p.rotation;}, lastCheckedTime);
        auto positions = getPropertiesNullable<NEVector::Vector3>(tracks, [](Properties const& p) {return p.position;}, lastCheckedTime);
        auto localPositions = getPropertiesNullable<NEVector::Vector3>(tracks, [](Properties const& p) {return p.localPosition;}, lastCheckedTime);
        auto scales = getPropertiesNullable<NEVector::Vector3>(tracks, [](Properties const& p) {return p.scale;}, lastCheckedTime);

        combine(localRotation, localRotations, *);
        combine(rotation, rotations, *);
        combine(scale, scales, +);
        combine(position, positions, +);
        combine(localPosition, localPositions, +);
    }


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

void Chroma::GameObjectTrackController::Init(std::vector<Track*> track, float noteLinesDistance,
                                             GlobalNamespace::TrackLaneRing * trackLaneRing,
                                             GlobalNamespace::ParametricBoxController * parametricBoxController,
                                             GlobalNamespace::BeatmapObjectsAvoidance * beatmapObjectsAvoidance, bool v2) {
    CRASH_UNLESS(!track.empty());
    this->data = &_dataMap.try_emplace(nextId, track, trackLaneRing, parametricBoxController, beatmapObjectsAvoidance, noteLinesDistance, v2).first->second;
    nextId++;
}

void Chroma::GameObjectTrackController::HandleTrackData(UnityEngine::GameObject *gameObject,
                                                        std::vector<Track*> track,
                                                        float noteLinesDistance,
                                                        GlobalNamespace::TrackLaneRing * trackLaneRing,
                                                        GlobalNamespace::ParametricBoxController * parametricBoxController,
                                                        GlobalNamespace::BeatmapObjectsAvoidance * beatmapObjectsAvoidance, bool v2) {
    auto* existingTrackController = gameObject->GetComponent<GameObjectTrackController*>();
    if (existingTrackController)
    {
        Destroy(existingTrackController);
    }

    if (!track.empty())
    {
        auto* trackController = gameObject->AddComponent<GameObjectTrackController*>();
        trackController->Init(track, noteLinesDistance, trackLaneRing, parametricBoxController, beatmapObjectsAvoidance, v2);

        for (auto t : track)
            t->AddGameObject(gameObject);
    }
}
