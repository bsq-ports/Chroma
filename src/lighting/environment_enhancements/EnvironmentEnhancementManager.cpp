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

#include <concepts>
#include <regex>

#include "tracks/shared/Animation/PointDefinition.h"
#include "tracks/shared/AssociatedData.h"

using namespace Chroma;
using namespace ChromaUtils;
using namespace UnityEngine::SceneManagement;

std::vector<GameObjectInfo>
Chroma::EnvironmentEnhancementManager::LookupId(const std::string& id, Chroma::LookupMethod lookupMethod) {
    std::function < bool(GameObjectInfo) > predicate;

    std::string lookupMethodStr = "";
//    Func<GameObjectInfo, bool> predicate;

    try {
        switch (lookupMethod) {
            case LookupMethod::Regex: {
                lookupMethodStr = "Regex";
                std::regex regex(id, std::regex_constants::ECMAScript | std::regex_constants::optimize);
                predicate = [=](const GameObjectInfo &n) {
                    return std::regex_search(n.FullID, regex);
                };
                break;
            }

            case LookupMethod::Exact: {
                lookupMethodStr = "Exact";
                predicate = [=](const GameObjectInfo &n) { return n.FullID == id; };
                break;
            }

            case LookupMethod::Contains: {
                lookupMethodStr = "Contains";
                predicate = [=](const GameObjectInfo &n) {
                    return n.FullID.find(id) != std::string::npos;
                };
                break;
            }

            default: {
                return {};
            }
        }
    } catch (std::exception &e) {
        getLogger().error("Failed to create match for lookup (%s) with id (%s)", lookupMethodStr.c_str(), id.c_str());
        getLogger().error("Error: %s", e.what());
    }

    std::vector<GameObjectInfo> ret;
    ret.reserve(_gameObjectInfos.size());

    for (const auto &o : _gameObjectInfos) {
        // We have a try/catch here so the loop doesn't die
        try {
            if (predicate(o))
                ret.push_back(o);
        } catch (std::exception &e) {
            getLogger().error("Failed to match (%s) for lookup (%s) with id (%s)", o.FullID.c_str(), lookupMethodStr.c_str(), id.c_str());
            getLogger().error("Error: %s", e.what());
        }
    }

    ret.shrink_to_fit();

    return ret;
}

std::optional<Sombrero::FastVector3>
EnvironmentEnhancementManager::GetVectorData(std::reference_wrapper<rapidjson::Value> dynDataW, const std::string& name) {
    rapidjson::Value& dynData = dynDataW.get();

    auto& objectsVal = dynData.FindMember(name.c_str())->value;

    if (!objectsVal.IsArray() || objectsVal.Empty())
        return std::nullopt;

    return Sombrero::FastVector3 {objectsVal[0].GetFloat(), objectsVal[1].GetFloat(), objectsVal[2].GetFloat()};
}

void EnvironmentEnhancementManager::GetAllGameObjects() {
    _gameObjectInfos.clear();

    auto gameObjectsAll = UnityEngine::Resources::FindObjectsOfTypeAll<UnityEngine::GameObject*>();
    std::vector<UnityEngine::GameObject*> gameObjectsVec;
    gameObjectsVec.reserve(gameObjectsAll->Length());


    for (int i = 0; i < gameObjectsAll->Length(); i++) {
        auto gameObject = gameObjectsAll->get(i);
        if (!gameObject) continue;

        auto sceneNameIl2cpp = gameObject->get_scene().get_name();
        if (!sceneNameIl2cpp) continue;

        std::string sceneName = to_utf8(csstrtostr(sceneNameIl2cpp));

        if ((sceneName.find("Environment") != std::string::npos && sceneName.find("Menu") == std::string::npos) || gameObject->GetComponent<GlobalNamespace::TrackLaneRing*>()) {
            gameObjectsVec.push_back(gameObject);
        }
    }

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

    for (auto& gameObject : gameObjectsVec2) {
        if (!gameObject) continue;

        _gameObjectInfos.emplace_back(gameObject);

        // seriously what the fuck beat games
        // GradientBackground permanently yeeted because it looks awful and can ruin multi-colored chroma maps
        if (to_utf8(csstrtostr(gameObject->get_name())) == "GradientBackground")
        {
            gameObject->SetActive(false);
        }
    }

    // Shrink if necessary
    _gameObjectInfos.shrink_to_fit();
}

void
EnvironmentEnhancementManager::Init(CustomJSONData::CustomBeatmapData *customBeatmapData, float noteLinesDistance) {
    getLogger().debug("Custom beat map %p", customBeatmapData);
    getLogger().debug("Custom beat map custom data %p", customBeatmapData->customData);
    auto customDynWrapper = customBeatmapData->customData->value;
    TracksAD::BeatmapAssociatedData& trackBeatmapAD = TracksAD::getBeatmapAD(customBeatmapData->customData);
    GameObjectTrackController::ClearData();
    if (customDynWrapper) {

        rapidjson::Value &dynData = *customDynWrapper;

        auto environmentData = dynData.FindMember(ENVIRONMENT);


        if (environmentData != dynData.MemberEnd()) {
            GetAllGameObjects();

            AvoidanceRotation.clear();
            AvoidancePosition.clear();
            RingRotationOffsets.clear();
            ParametricBoxControllerParameters::TransformParameters.clear();

            auto environmentDataObject = environmentData->value.GetArray();

            for (auto &gameObjectDataVal : environmentDataObject) {
                auto trackNameIt = gameObjectDataVal.FindMember(Chroma::TRACK);

                std::optional<std::string> trackName;

                if (trackNameIt != gameObjectDataVal.MemberEnd()) {
                    trackName = trackNameIt->value.GetString();
                    std::string val = *trackName;
                    if (trackBeatmapAD.tracks.find(val) == trackBeatmapAD.tracks.end())
                        trackBeatmapAD.tracks.emplace(val, Track());
                }




                auto idMember = gameObjectDataVal.FindMember(IDVAR);

                std::string id = idMember == gameObjectDataVal.MemberEnd() ? "" : idMember->value.GetString();
                std::string lookupString = gameObjectDataVal.FindMember(LOOKUPMETHOD)->value.GetString();

                // Convert string to lower case
                std::transform(lookupString.begin(), lookupString.end(), lookupString.begin(), ::tolower);
                LookupMethod lookupMethod;

                if (lookupString == "regex") {
                    lookupMethod = LookupMethod::Regex;
                } else if (lookupString == "exact") {
                    lookupMethod = LookupMethod::Exact;
                } else if (lookupString == "contains") {
                    lookupMethod = LookupMethod::Contains;
                }

                std::optional<int> dupeAmount = getIfExists<int>(gameObjectDataVal, DUPLICATIONAMOUNT);

                std::optional<bool> active = getIfExists<bool>(gameObjectDataVal, ACTIVE);
                auto scale = GetVectorData(gameObjectDataVal, SCALE);
                auto position = GetVectorData(gameObjectDataVal, POSITION);
                auto rotation = GetVectorData(gameObjectDataVal, OBJECTROTATION);
                auto localPosition = GetVectorData(gameObjectDataVal, LOCALPOSITION);
                auto localRotation = GetVectorData(gameObjectDataVal, LOCALROTATION);

                auto lightID = getIfExists<int>(gameObjectDataVal, LIGHTID);

                auto foundObjects = LookupId(id, lookupMethod);

                std::vector<GameObjectInfo> gameObjectInfos;
                if (dupeAmount) {
                    gameObjectInfos.reserve(_gameObjectInfos.size());

                    for (const auto &gameObjectInfo : foundObjects) {
                        if (getChromaConfig().PrintEnvironmentEnhancementDebug.GetValue()) {
                            getLogger().info("Duplicating [%s]:", gameObjectInfo.FullID.c_str());
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

                            ComponentInitializer::InitializeComponents(newGameObject->get_transform(),
                                                                       gameObject->get_transform(), _gameObjectInfos,
                                                                       componentDatas, lightID);
                            for (auto &o : _gameObjectInfos) {
                                if (o.GameObject->Equals(newGameObject)) {
                                    gameObjectInfos.push_back(o);
                                }
                            }


                        }
                    }
                } else {
                    if (lightID)
                    {
                        getLogger().error("LightID requested but no duplicated object to apply to.");
                    }

                    gameObjectInfos = foundObjects;
                }

                for (auto &gameObjectInfo : gameObjectInfos) {
                    auto gameObject = gameObjectInfo.GameObject;

                    if (active) {
                        gameObjectInfo.GameObject->SetActive(active.value());
                    }

                    auto transform = gameObject->get_transform();

                    if (scale) {
                        transform->set_localScale(scale.value());
                    }

                    if (position) {
                        transform->set_position(position.value() * noteLinesDistance);
                    }

                    if (rotation) {
                        transform->set_eulerAngles(rotation.value());
                    }

                    if (localPosition) {
                        transform->set_localPosition(localPosition.value() * noteLinesDistance);
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
                    GameObjectTrackController::HandleTrackData(gameObject, gameObjectDataVal, customBeatmapData, noteLinesDistance, ptrToOpt(trackLaneRing), ptrToOpt(parametricBoxController), ptrToOpt(beatmapObjectsAvoidance));


                    if (getChromaConfig().PrintEnvironmentEnhancementDebug.GetValue()) {
                        getLogger().info("ID [\"%s\"] using method [%s] found:", id.c_str(), lookupString.c_str());

                        for (const auto &o : foundObjects) {
                            getLogger().info("%s", o.FullID.c_str());
                        }

                        getLogger().info("=====================================");
                    }
                }
            }
        }
    }
    LegacyEnvironmentRemoval::Init(customBeatmapData);

}

void EnvironmentEnhancementManager::GetChildRecursive(UnityEngine::Transform *gameObject,
                                                      std::vector<UnityEngine::Transform *> &children) {
    children.reserve(children.size() + gameObject->get_childCount());
    for (int i = 0; i < gameObject->get_childCount(); i++) {
        auto child = gameObject->GetChild(i);
        children.push_back(child);
        GetChildRecursive(child, children);
    }
}


