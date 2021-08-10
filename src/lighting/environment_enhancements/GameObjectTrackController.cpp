#include "lighting/environment_enhancements/GameObjectTrackController.hpp"
#include "Chroma.hpp"
#include "utils/ChromaUtils.hpp"

#include "lighting/environment_enhancements/EnvironmentEnhancementManager.hpp"
#include "lighting/environment_enhancements/ParametricBoxControllerParameters.hpp"

#include "tracks/shared/Animation/Animation.h"

DEFINE_TYPE(Chroma, GameObjectTrackControllerData)
DEFINE_TYPE(Chroma, GameObjectTrackController)

template<typename T>
static std::optional<T> getPropertyNullable(const std::optional<PropertyValue>& prop) {
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



void Chroma::GameObjectTrackController::ClearData() {
    _dataMap.clear();
    _dataMap = {};
    nextId = 0;
}

void Chroma::GameObjectTrackController::ctor() {
    getLogger().info("Created %lld this %i", data, nextId);
}

void Chroma::GameObjectTrackController::Update() {
    if (!data) {
        auto it = _dataMap.find(id);

        if (it != _dataMap.end()) {
            data = it->second;
        }
    }

    if (!data){
        getLogger().error("Data is null! Should remove component or just early return? %p %s", this, to_utf8(csstrtostr(get_gameObject()->get_name())).c_str());
        Destroy(this);
        return;
    }
    auto& _noteLinesDistance = data->_noteLinesDistance;
    auto& _track = data->_track;
    // nullable
    auto& _trackLaneRing = data->_trackLaneRing;
    auto& _parametricBoxController = data->_parametricBoxController;
    auto& _beatmapObjectsAvoidance = data->_beatmapObjectsAvoidance;


    if (!_track){
        getLogger().error("Track is null! Should remove component or just early return? %p %s", this, to_utf8(csstrtostr(get_gameObject()->get_name())).c_str());
        Destroy(this);
        return;
    }
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
    CRASH_UNLESS(track);
    auto objectData = CRASH_UNLESS(il2cpp_utils::New<GameObjectTrackControllerData*>());
    objectData->_track = track;
    objectData->_noteLinesDistance = noteLinesDistance;
    objectData->_trackLaneRing = trackLaneRing;
    objectData->_parametricBoxController = parametricBoxController;
    objectData->_beatmapObjectsAvoidance = beatmapObjectsAvoidance;
    this->data = objectData;
    this->nextId = nextId;
    _dataMap[nextId] = data;
//    getLogger().info("Set (this: %p) track %p for %s", this, track, to_utf8(csstrtostr(get_gameObject()->get_name())).c_str());
    nextId++;
}


static std::optional<Track*> getTrack(rapidjson::Value& gameObjectData, CustomJSONData::CustomBeatmapData* beatmapData, std::string name = Chroma::TRACK) {
    auto tracks = TracksAD::getBeatmapAD(beatmapData->customData).tracks;

    auto trackName = gameObjectData.FindMember(name);

    if (trackName == gameObjectData.MemberEnd() || !trackName->value.IsString())
        return std::nullopt;

    auto trackFound = tracks.find(trackName->value.GetString());

    if (trackFound == tracks.end())
        return std::nullopt;

    Track& track = trackFound->second;

//    getLogger().info("Track ptr %p", &track);

    return ChromaUtils::ptrToOpt(&track);
}

void Chroma::GameObjectTrackController::HandleTrackData(UnityEngine::GameObject *gameObject,
                                                        rapidjson::Value& gameObjectData,
                                                        CustomJSONData::CustomBeatmapData * beatmapData,
                                                        float noteLinesDistance,
                                                        std::optional<GlobalNamespace::TrackLaneRing *> trackLaneRing,
                                                        std::optional<GlobalNamespace::ParametricBoxController *> parametricBoxController,
                                                        std::optional<GlobalNamespace::BeatmapObjectsAvoidance *> beatmapObjectsAvoidance) {
    auto track = getTrack(gameObjectData, beatmapData);

    if (track)
    {
//        getLogger().info("Making track for object %s yay! %p", to_utf8(csstrtostr(gameObject->get_name())).c_str(), track.value());
        GameObjectTrackController* trackController = gameObject->AddComponent<GameObjectTrackController*>();
        trackController->Init(track.value(), noteLinesDistance, trackLaneRing, parametricBoxController, beatmapObjectsAvoidance);
//        getLogger().debug("Did init for %p", trackController);
    }
}
