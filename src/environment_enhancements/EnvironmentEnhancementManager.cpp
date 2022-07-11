#include "main.hpp"
#include "environment_enhancements/EnvironmentEnhancementManager.hpp"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"

#include "utils/ChromaUtils.hpp"
#include "environment_enhancements/LegacyEnvironmentRemoval.hpp"
#include "environment_enhancements/ComponentInitializer.hpp"
#include "environment_enhancements/ParametricBoxControllerParameters.hpp"
#include "environment_enhancements/GameObjectTrackController.hpp"

#include <sstream>
#include <concepts>
#include "boost-regex/regex/include/boost/regex.hpp"

#include "tracks/shared/Animation/PointDefinition.h"
#include "tracks/shared/AssociatedData.h"
#include "environment_enhancements/MaterialAnimator.hpp"
#include "hooks/LightWithIdManager.hpp"

using namespace Chroma;
using namespace ChromaUtils;
using namespace UnityEngine::SceneManagement;

using ChromaRegex = boost::regex;

// We can return a reference here since _globalGameObjectInfos is keeping the reference alive
std::vector<ByRef<const GameObjectInfo>>
Chroma::EnvironmentEnhancementManager::LookupId(std::string_view const id, Chroma::LookupMethod lookupMethod) {
    std::vector<ByRef<const GameObjectInfo>> ret;
    ret.reserve(_globalGameObjectInfos.size());



    auto doLookup = [&ret, &id](auto&& lookupMethodStr, auto&& predicate) constexpr {
        for (auto const &o : _globalGameObjectInfos) {
            // We have a try/catch here so the loop doesn't die
            try {
                if (predicate(o))
                    ret.emplace_back(o);
            } catch (std::exception &e) {
                getLogger().error("Failed to match (%s) for lookup (%s) with id (%s)", o.FullID.c_str(), lookupMethodStr, id.data());
                getLogger().error("Error: %s", e.what());
            }
        }
    };



    // only set when needed
    ChromaRegex regex;

    std::string_view lookupMethodStr;

    try {
        switch (lookupMethod) {
            case LookupMethod::Regex: {
                lookupMethodStr = "Regex";
                regex = ChromaRegex(id.data(), boost::regex_constants::ECMAScript | boost::regex_constants::optimize);
                doLookup("Regex", [&regex](const GameObjectInfo &n) constexpr {
                    return boost::regex_search(n.FullID, regex);
                });
                break;
            }

            case LookupMethod::Exact: {
                lookupMethodStr = "Exact";
                size_t idHash = std::hash<std::string_view>()(id);
                doLookup("Exact", [idHash](const GameObjectInfo &n) constexpr { return n.FullIDHash == idHash; });
                break;
            }

            case LookupMethod::Contains: {
                doLookup("Contains", [&id](const GameObjectInfo &n) constexpr {
                    return n.FullID.find(id) != std::string::npos;
                });
                break;
            }

            case LookupMethod::StartsWith: {
                lookupMethodStr = "StartsWith";
                doLookup("StartsWith", [&id](const GameObjectInfo &n) constexpr {
                    return n.FullID.starts_with(id);
                });
                break;
            }

            case LookupMethod::EndsWith: {
                lookupMethodStr = "EndsWith";

                doLookup("EndsWith", [&id](const GameObjectInfo &n) constexpr {
                    return n.FullID.ends_with(id);
                });
                break;
            }

            default: {
                return {};
            }
        }
    } catch (std::exception const& e) {
        getLogger().error("Failed to create match for lookup (%s) with id (%s)", lookupMethodStr.data(), id.data());
        getLogger().error("Error: %s", e.what());
    }


    ret.shrink_to_fit();

    return ret;
}

static std::optional<Sombrero::FastVector3>
GetVectorData(const rapidjson::Value &dynData, const std::string_view name) {
    auto objectsValIt = dynData.FindMember(name.data());

    if (objectsValIt == dynData.MemberEnd())
        return std::nullopt;

    auto const& objectsVal = objectsValIt->value;

    if (!objectsVal.IsArray() || objectsVal.Empty())
        return std::nullopt;

    return Sombrero::FastVector3 {objectsVal[0].GetFloat(), objectsVal[1].GetFloat(), objectsVal[2].GetFloat()};
}

struct TransformData {
public:
    std::optional<Sombrero::FastVector3> scale;
    std::optional<Sombrero::FastVector3> position;
    std::optional<Sombrero::FastVector3> rotation;
    std::optional<Sombrero::FastVector3> localPosition;
    std::optional<Sombrero::FastVector3> localRotation;

    TransformData(TransformData&&) = default;

    TransformData(rapidjson::Value const& customData, bool v2) {
        scale = GetVectorData(customData, v2 ? NewConstants::V2_SCALE : NewConstants::SCALE);
        position = GetVectorData(customData, v2 ? NewConstants::V2_POSITION : NewConstants::POSITION);
        rotation = GetVectorData(customData, v2 ? NewConstants::V2_ROTATION : NewConstants::ROTATION);
        localPosition = GetVectorData(customData, v2 ? NewConstants::V2_LOCAL_POSITION : NewConstants::LOCAL_POSITION);
        localRotation = GetVectorData(customData, v2 ? NewConstants::V2_LOCAL_ROTATION : NewConstants::LOCAL_ROTATION);
    }

    inline void Apply(UnityEngine::Transform* transform, bool leftHanded, bool v2, float noteLinesDistance) const
    {
        auto position = this->position;
        auto localPosition = this->localPosition;
        if (v2)
        {
            // ReSharper disable once UseNullPropagation
            if (position)
            {
                position = position.value() * noteLinesDistance;
            }

            // ReSharper disable once UseNullPropagation
            if (localPosition)
            {
                localPosition = localPosition.value() * noteLinesDistance;
            }
        }

        Apply(transform, leftHanded, scale, position, rotation, localPosition, localRotation, v2);
    }


    inline void Apply(UnityEngine::Transform* transform, bool leftHanded, bool v2) const
    {
        Apply(transform, leftHanded, scale, position, rotation, localPosition, localRotation, v2);
    }

private:
    static void Apply(
            UnityEngine::Transform* transform,
    bool leftHanded,
            std::optional<Sombrero::FastVector3> scale,
            std::optional<Sombrero::FastVector3> position,
            std::optional<Sombrero::FastVector3> rotation,
            std::optional<Sombrero::FastVector3> localPosition,
            std::optional<Sombrero::FastVector3> localRotation,
            bool v2
            )
    {
        // TODO: Mirror
//        if (leftHanded)
//        {
//            scale = scale?.Mirror();
//            position = position?.Mirror();
//            rotation = rotation?.Mirror();
//            localPosition = localPosition?.Mirror();
//            localRotation = localRotation?.Mirror();
//        }

        if (scale)
        {
            transform->set_localScale(*scale);
        }

        if (localPosition)
        {
            transform->set_localPosition(*localPosition);
        }else if (position)
        {
            transform->set_position(*position);
        }

        if (localRotation)
        {
            transform->set_localEulerAngles(*localRotation);
        } else if (rotation)
        {
            transform->set_eulerAngles(*rotation);
        }
    }
};

void EnvironmentEnhancementManager::GetAllGameObjects() {
    LightIdRegisterer::canUnregister = true;

    _globalGameObjectInfos.clear();

    auto gameObjectsAll = UnityEngine::Resources::FindObjectsOfTypeAll<UnityEngine::GameObject*>();
    std::vector<UnityEngine::GameObject*> gameObjectsVec;
    gameObjectsVec.reserve(gameObjectsAll.Length());

    // I'll probably revist this formula for getting objects by only grabbing the root objects and adding all the children
    for (int i = 0; i < gameObjectsAll.Length(); i++) {
        auto gameObject = gameObjectsAll.get(i);
        if (!gameObject) continue;

        auto sceneNameIl2cpp = gameObject->get_scene().get_name();
        if (!sceneNameIl2cpp) continue;

        std::u16string_view sceneName = sceneNameIl2cpp;

        if ((sceneName.find(u"Environment") != std::string::npos && sceneName.find(u"Menu") == std::string::npos) || gameObject->GetComponent<GlobalNamespace::TrackLaneRing*>()) {
            gameObjectsVec.emplace_back(gameObject);
        }
    }

    // Adds the children of whitelist GameObjects
    // Mainly for grabbing cone objects in KaleidoscopeEnvironment
    std::vector<UnityEngine::GameObject*> gameObjectsVec2(gameObjectsVec.begin(), gameObjectsVec.end());
    for (auto& gameObject : gameObjectsVec) {
        std::vector<UnityEngine::Transform*> allChildren;
        GetChildRecursive(gameObject->get_transform(), allChildren);

        for (auto &transform : allChildren) {
            auto childGameObject = transform->get_gameObject();
            if (std::find(gameObjectsVec.begin(), gameObjectsVec.end(), childGameObject) == gameObjectsVec.end()) {
                gameObjectsVec2.push_back(childGameObject);
            }
        }
    }

    for (auto const& gameObject : gameObjectsVec2) {
        if (!gameObject) continue;

        _globalGameObjectInfos.emplace_back(gameObject);

        // seriously what the fuck beat games
        // GradientBackground permanently yeeted because it looks awful and can ruin multi-colored chroma maps
        if (gameObject->get_name() == "GradientBackground")
        {
            gameObject->SetActive(false);
        }
    }

    // Shrink if necessary
    _globalGameObjectInfos.shrink_to_fit();

    if (getChromaConfig().PrintEnvironmentEnhancementDebug.GetValue()) {
        // print objects
        std::thread([] {
            // copy
            auto objectsToPrint = std::vector<GameObjectInfo>(_globalGameObjectInfos);

            // Sort in order from shortest to longest string
            std::sort(objectsToPrint.begin(), objectsToPrint.end(),
                      [](GameObjectInfo const &a, GameObjectInfo const &b) {
                          return a.FullID < b.FullID;
                      });

            std::stringstream ss;
            for (auto const &o: objectsToPrint) {
                ss << o.FullID << std::endl;
            }

            getLogger().info("Objects found in environment:\n%s", ss.str().c_str());
        }).detach();
    }
}

void
EnvironmentEnhancementManager::Init(CustomJSONData::CustomBeatmapData *customBeatmapData, float noteLinesDistance) {
    getLogger().debug("Custom beat map %p", customBeatmapData);
    getLogger().debug("Custom beat map custom data %p", customBeatmapData->customData);
    auto customDynWrapper = customBeatmapData->customData->value;
    bool v2 = customBeatmapData->v2orEarlier;
    TracksAD::BeatmapAssociatedData& trackBeatmapAD = TracksAD::getBeatmapAD(customBeatmapData->customData);
    GameObjectTrackController::LeftHanded = trackBeatmapAD.leftHanded;
    bool leftHanded = trackBeatmapAD.leftHanded;
    GameObjectTrackController::ClearData();

    AvoidanceRotation.clear();
    AvoidancePosition.clear();
    RingRotationOffsets.clear();
    ParametricBoxControllerParameters::TransformParameters.clear();

    if (customDynWrapper) {

        rapidjson::Value const& dynData = *customDynWrapper;

        MaterialsManager materialsManager(dynData, trackBeatmapAD, v2);
        GeometryFactory geometryFactory(materialsManager, v2);

        auto environmentData = dynData.FindMember(v2 ? NewConstants::V2_ENVIRONMENT.data() : NewConstants::ENVIRONMENT.data());


        if (environmentData != dynData.MemberEnd()) {
            GetAllGameObjects();


            std::vector<Profiler> profileData;
            auto environmentDataObject = environmentData->value.GetArray();

            // Record start time
            auto startAll = std::chrono::high_resolution_clock::now();

            for (auto const& gameObjectDataVal : environmentDataObject) {
                // Record start time
                auto& profiler = profileData.emplace_back();
                profiler.startTimer();

                auto idMember = gameObjectDataVal.FindMember(v2 ? NewConstants::V2_GAMEOBJECT_ID.data() : NewConstants::GAMEOBJECT_ID.data());
                auto geometryMember = gameObjectDataVal.FindMember(v2 ? NewConstants::V2_GEOMETRY.data() : NewConstants::GEOMETRY.data());
                std::vector<ByRef<const GameObjectInfo>> foundObjects;


                if (idMember != gameObjectDataVal.MemberEnd()){
                    if (geometryMember != gameObjectDataVal.MemberEnd()) continue;

                    std::string_view id = idMember->value.GetString();
                    std::string lookupString = gameObjectDataVal.FindMember(v2 ? NewConstants::V2_LOOKUP_METHOD.data() : NewConstants::LOOKUP_METHOD.data())->value.GetString();

                    // Convert string to lower case
                    std::transform(lookupString.begin(), lookupString.end(), lookupString.begin(), ::tolower);
                    LookupMethod lookupMethod = LookupMethod::Exact;

                    // Record JSON parse time
                    profiler.mark("Parsing JSON for id " + std::string(id));

                    if (lookupString == "regex") {
                        lookupMethod = LookupMethod::Regex;
                    } else if (lookupString == "exact") {
                        lookupMethod = LookupMethod::Exact;
                    } else if (lookupString == "contains") {
                        lookupMethod = LookupMethod::Contains;
                    }else if (lookupString == "startswith") {
                        lookupMethod = LookupMethod::StartsWith;
                    }else if (lookupString == "endswith") {
                        lookupMethod = LookupMethod::EndsWith;
                    }

                    foundObjects = LookupId(id, lookupMethod);

                    // Record find object time
                    std::stringstream foundObjectsLog;
                    foundObjectsLog << "Finding objects for id (" << std::to_string(foundObjects.size()) << ") "<< id << " using " << lookupString;

                    if (getChromaConfig().PrintEnvironmentEnhancementDebug.GetValue()) {
                        getLogger().info("ID [\"%s\"] using method [%s] found:", id.data(), lookupString.c_str());
                    }

                    profiler.mark(foundObjectsLog.str());

                } else if (geometryMember != gameObjectDataVal.MemberEnd()) {
                    auto goInfo = ByRef<const GameObjectInfo>(_globalGameObjectInfos.emplace_back(geometryFactory.Create(geometryMember->value)));
                    // Record JSON parse time
                    profiler.mark("Parsing JSON for geometry ");

                    foundObjects.emplace_back(goInfo);
                } else continue;


                if (foundObjects.empty()) {
                    profiler.mark("No objects found!", false);
                    profiler.endTimer();
                    continue;
                }

                if (getChromaConfig().PrintEnvironmentEnhancementDebug.GetValue()) {
                    for (const auto &o : foundObjects) {
                        getLogger().info("%s", o.heldRef.FullID.c_str());
                    }

                    getLogger().info("=====================================");
                }

                TransformData spawnData(gameObjectDataVal, v2);

                std::optional<int> dupeAmount = getIfExists<int>(gameObjectDataVal, v2 ? NewConstants::V2_DUPLICATION_AMOUNT : NewConstants::DUPLICATION_AMOUNT);

                std::optional<bool> active = getIfExists<bool>(gameObjectDataVal, v2 ? NewConstants::V2_ACTIVE : NewConstants::ACTIVE);
                auto lightID = getIfExists<int>(gameObjectDataVal, v2 ? NewConstants::V2_LIGHT_ID : NewConstants::LIGHT_ID);

                // Create track if objects are found
                auto trackNameIt = gameObjectDataVal.FindMember(v2 ? Chroma::NewConstants::V2_TRACK.data() : Chroma::NewConstants::TRACK.data());

                std::optional<std::string> trackName;
                std::optional<Track*> track;

                if (trackNameIt != gameObjectDataVal.MemberEnd()) {
                    trackName = trackNameIt->value.GetString();
                    std::string val = *trackName;
                    track = &(trackBeatmapAD.tracks.try_emplace(val, v2).first->second);
                }


                std::vector<ByRef<const GameObjectInfo>> gameObjectInfos;
                if (dupeAmount) {
                    gameObjectInfos.reserve(foundObjects.size() * dupeAmount.value());

                    for (const auto &gameObjectInfoRef: foundObjects) {
                        const auto &gameObjectInfo = gameObjectInfoRef.heldRef;
                        if (getChromaConfig().PrintEnvironmentEnhancementDebug.GetValue()) {
                            profiler.mark("Duplicating [" + gameObjectInfo.FullID + "]:", false);
                        }

                        auto gameObject = gameObjectInfo.GameObject;
                        auto parent = gameObject->get_transform()->get_parent();
                        auto scene = gameObject->get_scene();

                        for (int i = 0; i < dupeAmount.value(); i++) {
                            std::vector<std::shared_ptr<IComponentData>> componentDatas;
                            ComponentInitializer::PrefillComponentsData(gameObject->get_transform(),
                                                                        componentDatas);
                            auto newGameObject = UnityEngine::Object::Instantiate(gameObject);
                            ComponentInitializer::PostfillComponentsData(newGameObject->get_transform(),
                                                                         gameObject->get_transform(),
                                                                         componentDatas);

                            SceneManager::MoveGameObjectToScene(newGameObject, scene);
                            newGameObject->get_transform()->SetParent(parent, true);

                            auto const &newGameObjectInfo = ComponentInitializer::InitializeComponents(
                                    newGameObject->get_transform(),
                                    gameObject->get_transform(), _globalGameObjectInfos,
                                    componentDatas);
                            gameObjectInfos.emplace_back(newGameObjectInfo);
                            // This is not needed as long as InitializeComponents adds to gameObjectInfos

//                            for (auto const& o : _globalGameObjectInfos) {
//                                if (o.GameObject->Equals(newGameObject)) {
//                                    gameObjectInfos.emplace_back(o);
//                                }
//                            }


                        }
                    }
                    // Record end time
                    profiler.mark("Duping ");
                } else {
                    if (lightID) {
                        getLogger().error("LightID requested but no duplicated object to apply to.");
                    }

                    // Better way of doing this?
                    // For some reason, copy constructor is deleted?
                    gameObjectInfos = std::move(foundObjects);
                }



                for (auto const& gameObjectInfoRef : gameObjectInfos) {
                    const auto &gameObjectInfo = gameObjectInfoRef.heldRef;

                    auto gameObject = gameObjectInfo.GameObject;

                    if (active) {
                        gameObject->SetActive(active.value());
                    }

                    auto transform = gameObject->get_transform();

                    spawnData.Apply(transform, leftHanded, v2, noteLinesDistance);
                    auto const& position = spawnData.position;
                    auto const& localPosition = spawnData.localPosition;
                    auto const& rotation = spawnData.rotation;
                    auto const& localRotation = spawnData.localRotation;
                    auto const& scale = spawnData.scale;

                    // Handle TrackLaneRing
                    auto trackLaneRing = gameObject->GetComponent<GlobalNamespace::TrackLaneRing *>();
                    if (trackLaneRing != nullptr) {
                        if (position || localPosition) {
                            trackLaneRing->positionOffset = transform->get_localPosition();
                            trackLaneRing->posZ = 0;
                        }

                        if (rotation || localRotation) {
                            RingRotationOffsets[trackLaneRing] = transform->get_localRotation();
                            trackLaneRing->rotZ = 0;
                        }
                    }

                    // Handle ParametricBoxController
                    auto parametricBoxController = gameObject->GetComponent<GlobalNamespace::ParametricBoxController*>();
                    if (parametricBoxController != nullptr)
                    {
                        if (position || localPosition)
                        {
                            ParametricBoxControllerParameters::SetTransformPosition(parametricBoxController, transform->get_localPosition());
                        }

                        if (scale)
                        {
                            ParametricBoxControllerParameters::SetTransformScale(parametricBoxController, transform->get_localScale());
                        }
                    }

                    auto* beatmapObjectsAvoidance = gameObject->GetComponent<GlobalNamespace::BeatmapObjectsAvoidance*>();

                    if (beatmapObjectsAvoidance) {
                        if (position || localPosition) {
                            AvoidancePosition[beatmapObjectsAvoidance] = transform->get_localPosition();
                        }

                        if (rotation || localRotation) {
                            AvoidanceRotation[beatmapObjectsAvoidance] = transform->get_localRotation();
                        }
                     }

                    ComponentInitializer::InitializeLights(gameObject, gameObjectDataVal, v2);

                    GameObjectTrackController::HandleTrackData(gameObject, track, noteLinesDistance, trackLaneRing, parametricBoxController, beatmapObjectsAvoidance, v2);
                }



                // Record end time
                profiler.endTimer();
            }

            // Record all end time
            auto finish = std::chrono::high_resolution_clock::now();
            auto millisElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - startAll).count();

            std::thread([profileData = move(profileData), millisElapsed]{
                // Log all objects
                for (auto const& profile : profileData) {
                    profile.printMarks();
                    getLogger().info("=====================================\n");
                }



                getLogger().info("Finished environment enhancements took %lldms", millisElapsed);

            }).detach();
        }

        auto const& materials = materialsManager.GetMaterials();
        std::vector<MaterialInfo> animatedMaterials;
        animatedMaterials.reserve(materials.size());

        for (auto const& [s, m] : materials) {
            if (!m.Track || m.Track->empty()) continue;
            animatedMaterials.emplace_back(m);
        }

        if (!animatedMaterials.empty()) {
            UnityEngine::GameObject::New_ctor("MaterialAnimator")->AddComponent<MaterialAnimator*>()->materials = std::move(animatedMaterials);
        }
    }
    if (v2) {
        LegacyEnvironmentRemoval::Init(customBeatmapData);
    }
}

void EnvironmentEnhancementManager::GetChildRecursive(UnityEngine::Transform *gameObject,
                                                      std::vector<UnityEngine::Transform *> &children) {
    children.reserve(children.size() + gameObject->get_childCount());
    auto gameObjectChildCount = gameObject->get_childCount();
    for (int i = 0; i < gameObjectChildCount; i++) {
        auto child = gameObject->GetChild(i);
        children.push_back(child);
        GetChildRecursive(child, children);
    }
}


