#include "main.hpp"
#include "lighting/environment_enhancements/EnvironmentEnhancementManager.hpp"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"

#include "utils/ChromaUtils.hpp"
#include "lighting/environment_enhancements/LegacyEnvironmentRemoval.hpp"
#include "lighting/environment_enhancements/ComponentInitializer.hpp"
#include "lighting/environment_enhancements/ParametricBoxControllerParameters.hpp"
#include "lighting/environment_enhancements/GameObjectTrackController.hpp"

#include <sstream>
#include <concepts>
#include "boost-regex/regex/include/boost/regex.hpp"

#include "tracks/shared/Animation/PointDefinition.h"
#include "tracks/shared/AssociatedData.h"

using namespace Chroma;
using namespace ChromaUtils;
using namespace UnityEngine::SceneManagement;

using ChromaRegex = boost::regex;

// We can return a reference here since _globalGameObjectInfos is keeping the reference alive
std::vector<ByRef<const GameObjectInfo>>
Chroma::EnvironmentEnhancementManager::LookupId(std::string_view const id, Chroma::LookupMethod lookupMethod) {
    std::vector<ByRef<const GameObjectInfo>> ret;
    ret.reserve(_globalGameObjectInfos.size());



    auto doLookup = [&ret, &id](std::string const& lookupMethodStr, auto&& predicate) constexpr {
        for (auto const &o : _globalGameObjectInfos) {
            // We have a try/catch here so the loop doesn't die
            try {
                if (predicate(o))
                    ret.emplace_back(o);
            } catch (std::exception &e) {
                getLogger().error("Failed to match (%s) for lookup (%s) with id (%s)", o.FullID.c_str(), lookupMethodStr.c_str(), id.data());
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
    } catch (std::exception &e) {
        getLogger().error("Failed to create match for lookup (%s) with id (%s)", lookupMethodStr.data(), id.data());
        getLogger().error("Error: %s", e.what());
    }


    ret.shrink_to_fit();

    return ret;
}

std::optional<Sombrero::FastVector3>
EnvironmentEnhancementManager::GetVectorData(const rapidjson::Value &dynData, const std::string_view name) {
    auto objectsValIt = dynData.FindMember(name.data());

    if (objectsValIt == dynData.MemberEnd())
        return std::nullopt;

    auto const& objectsVal = objectsValIt->value;

    if (!objectsVal.IsArray() || objectsVal.Empty())
        return std::nullopt;

    return Sombrero::FastVector3 {objectsVal[0].GetFloat(), objectsVal[1].GetFloat(), objectsVal[2].GetFloat()};
}

void EnvironmentEnhancementManager::GetAllGameObjects() {
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

        std::string sceneName = sceneNameIl2cpp;

        if ((sceneName.find("Environment") != std::string::npos && sceneName.find("Menu") == std::string::npos) || gameObject->GetComponent<GlobalNamespace::TrackLaneRing*>()) {
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
    GameObjectTrackController::ClearData();

    AvoidanceRotation.clear();
    AvoidancePosition.clear();
    RingRotationOffsets.clear();
    ParametricBoxControllerParameters::TransformParameters.clear();

    if (customDynWrapper) {

        rapidjson::Value const& dynData = *customDynWrapper;

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

                if (idMember == gameObjectDataVal.MemberEnd()) continue;

                std::string_view id = idMember->value.GetString();
                std::string lookupString = gameObjectDataVal.FindMember(v2 ? NewConstants::V2_LOOKUP_METHOD.data() : NewConstants::LOOKUP_METHOD.data())->value.GetString();

                // Convert string to lower case
                std::transform(lookupString.begin(), lookupString.end(), lookupString.begin(), ::tolower);
                LookupMethod lookupMethod = LookupMethod::Exact;

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

                std::optional<int> dupeAmount = getIfExists<int>(gameObjectDataVal, v2 ? NewConstants::V2_DUPLICATION_AMOUNT : NewConstants::DUPLICATION_AMOUNT);

                std::optional<bool> active = getIfExists<bool>(gameObjectDataVal, v2 ? NewConstants::V2_ACTIVE : NewConstants::ACTIVE);
                auto scale = GetVectorData(gameObjectDataVal, v2 ? NewConstants::V2_SCALE : NewConstants::SCALE);
                auto position = GetVectorData(gameObjectDataVal, v2 ? NewConstants::V2_POSITION : NewConstants::POSITION);
                auto rotation = GetVectorData(gameObjectDataVal, v2 ? NewConstants::V2_ROTATION : NewConstants::ROTATION);
                auto localPosition = GetVectorData(gameObjectDataVal, v2 ? NewConstants::V2_LOCAL_POSITION : NewConstants::LOCAL_POSITION);
                auto localRotation = GetVectorData(gameObjectDataVal, v2 ? NewConstants::V2_LOCAL_ROTATION : NewConstants::LOCAL_ROTATION);

                if (v2) {
                    if (position) {
                        position = *position * noteLinesDistance;
                    }

                    if (rotation) {
                        rotation = *rotation * noteLinesDistance;
                    }
                }

                auto lightID = getIfExists<int>(gameObjectDataVal, v2 ? NewConstants::V2_LIGHT_ID : NewConstants::LIGHT_ID);

                // Record JSON parse time
                profiler.mark("Parsing JSON for id " + std::string(id));

                std::vector<ByRef<const GameObjectInfo>> const foundObjects(LookupId(id, lookupMethod));

                // Record find object time
                std::stringstream foundObjectsLog;
                foundObjectsLog << "Finding objects for id (" << std::to_string(foundObjects.size()) << ") "<< id << " using " << lookupString;

                profiler.mark(foundObjectsLog.str());

                if (foundObjects.empty()) {
                    profiler.mark("No objects found!", false);
                    profiler.endTimer();
                    continue;
                }

                // Create track if objects are found
                auto trackNameIt = gameObjectDataVal.FindMember(Chroma::TRACK.data());

                std::optional<std::string> trackName;
                std::optional<Track*> track;

                if (trackNameIt != gameObjectDataVal.MemberEnd()) {
                    trackName = trackNameIt->value.GetString();
                    std::string val = *trackName;
                    track = &(trackBeatmapAD.tracks.try_emplace(val).first->second);
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
                                    componentDatas, lightID);
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
                    profiler.mark("Duping for id " + lookupString);
                } else {
                    if (lightID) {
                        getLogger().error("LightID requested but no duplicated object to apply to.");
                    }

                    // Better way of doing this?
                    // For some reason, copy constructor is deleted?
                    gameObjectInfos = std::vector(foundObjects);
                }



                for (auto const& gameObjectInfoRef : gameObjectInfos) {
                    const auto &gameObjectInfo = gameObjectInfoRef.heldRef;

                    auto gameObject = gameObjectInfo.GameObject;

                    if (active) {
                        gameObject->SetActive(active.value());
                    }

                    auto transform = gameObject->get_transform();

                    if (scale) {
                        transform->set_localScale(scale.value());
                    }

                    if (position) {
                        transform->set_position(position.value());
                    }

                    if (rotation) {
                        transform->set_eulerAngles(rotation.value());
                    }

                    if (localPosition) {
                        transform->set_localPosition(localPosition.value());
                    }

                    if (localRotation) {
                        transform->set_localEulerAngles(localRotation.value());
                    }

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

                    GlobalNamespace::BeatmapObjectsAvoidance* beatmapObjectsAvoidance = gameObject->GetComponent<GlobalNamespace::BeatmapObjectsAvoidance*>();

                    if (beatmapObjectsAvoidance) {
                        if (position || localPosition) {
                            AvoidancePosition[beatmapObjectsAvoidance] = transform->get_localPosition();
                        }

                        if (rotation || localRotation) {
                            AvoidanceRotation[beatmapObjectsAvoidance] = transform->get_localRotation();
                        }
                     }
                    GameObjectTrackController::HandleTrackData(gameObject, track, noteLinesDistance, trackLaneRing, parametricBoxController, beatmapObjectsAvoidance, v2);
                }

                if (getChromaConfig().PrintEnvironmentEnhancementDebug.GetValue()) {
                    getLogger().info("ID [\"%s\"] using method [%s] found:", id.data(), lookupString.c_str());

                    for (const auto &o : foundObjects) {
                        getLogger().info("%s", o.heldRef.FullID.c_str());
                    }

                    getLogger().info("=====================================");
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


