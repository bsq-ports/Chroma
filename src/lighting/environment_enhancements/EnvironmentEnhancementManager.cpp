#include "main.hpp"
#include "lighting/environment_enhancements/EnvironmentEnhancementManager.hpp"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"

#include "utils/ChromaUtils.hpp"
#include "lighting/environment_enhancements/LegacyEnvironmentRemoval.hpp"
#include "lighting/environment_enhancements/ComponentInitializer.hpp"

#include <concepts>
#include <regex>

using namespace Chroma;
using namespace ChromaUtils;
using namespace UnityEngine::SceneManagement;

std::vector<GameObjectInfo>
Chroma::EnvironmentEnhancementManager::LookupId(const std::string& id, Chroma::LookupMethod lookupMethod) {
    std::function < bool(GameObjectInfo) > predicate;


//    Func<GameObjectInfo, bool> predicate;
    switch (lookupMethod) {
        case LookupMethod::Regex: {

            predicate = [=](const GameObjectInfo &n) {
                std::regex regex(id);

                return std::regex_match(n.FullID, regex);
            };
            break;
        }

        case LookupMethod::Exact: {
            predicate = [=](const GameObjectInfo &n) { return n.FullID == id;};
            break;
        }

        case LookupMethod::Contains: {
            predicate = [=](const GameObjectInfo &n) {
                return n.FullID.find(id) != std::string::npos; };
            break;
        }

        default: {
            return {};
        }
    }

    std::vector<GameObjectInfo> ret;

    for (const auto &o : _gameObjectInfos) {
        if(predicate(o))
            ret.push_back(o);
    }

    return ret;
}

std::optional<UnityEngine::Vector3>
EnvironmentEnhancementManager::GetVectorData(std::reference_wrapper<rapidjson::Value> dynDataW, const std::string& name) {
    rapidjson::Value& dynData = dynDataW.get();

    auto& objectsVal = dynData.FindMember(name.c_str())->value;

    if (!objectsVal.IsArray() || objectsVal.Empty())
        return std::nullopt;

    return UnityEngine::Vector3 {objectsVal[0].GetFloat(), objectsVal[1].GetFloat(), objectsVal[2].GetFloat()};
}

void EnvironmentEnhancementManager::GetAllGameObjects() {
    _gameObjectInfos = std::vector<GameObjectInfo>();

    auto gameObjects = UnityEngine::Resources::FindObjectsOfTypeAll<UnityEngine::GameObject*>();
    std::vector<UnityEngine::GameObject*> gameObjectsVec;


    for (int i = 0; i < gameObjects->Length(); i++) {
        auto gameObject = gameObjects->values[i];

        if (gameObject) {
            auto sceneName = gameObject->get_scene().get_name();

            if (sceneName && (sceneName->Contains(il2cpp_utils::createcsstr("Environment")) && !sceneName->Contains(il2cpp_utils::createcsstr("Menu")) || gameObject->GetComponent<GlobalNamespace::TrackLaneRing*>())) {
                gameObjectsVec.push_back(gameObject);
            }
        }
    }

    std::vector<UnityEngine::GameObject*> gameObjectsVec2(gameObjectsVec.begin(), gameObjectsVec.end());
    for (auto& gameObject : gameObjectsVec) {
        std::vector<UnityEngine::Transform*> allChildren;
        GetChildRecursive(gameObject->get_transform(), allChildren);

        for (auto &transform : allChildren) {
            getLogger().debug("Iterated %s", to_utf8(csstrtostr(transform->get_gameObject()->get_name())).c_str());
            if (!gameObjects->Contains(transform->get_gameObject())) {
                gameObjectsVec2.push_back(transform->get_gameObject());
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
}

void
EnvironmentEnhancementManager::Init(CustomJSONData::CustomBeatmapData *customBeatmapData, float noteLinesDistance) {
    getLogger().debug("Custom beat map %p", customBeatmapData);
    getLogger().debug("Custom beat map custom data %p", customBeatmapData->customData);
    auto customDynWrapper = customBeatmapData->customData->value;
    if (customDynWrapper) {

        rapidjson::Value &dynData = *customDynWrapper;

        auto environmentData = dynData.FindMember(ENVIRONMENT);


        if (environmentData != dynData.MemberEnd()) {
            GetAllGameObjects();

            SkipRingUpdate.clear();
            RingRotationOffsets.clear();

            auto environmentDataObject = environmentData->value.GetArray();

            for (auto &gameObjectDataVal : environmentDataObject) {
                auto idMember = gameObjectDataVal.FindMember(IDVAR);

                std::string id = idMember == gameObjectDataVal.MemberEnd() ? "" : idMember->value.GetString();
                std::string lookupString = gameObjectDataVal.FindMember(LOOKUPMETHOD)->value.GetString();

                // Convert string to lower case
                std::transform(lookupString.begin(), lookupString.end(), lookupString.begin(), ::tolower);
                LookupMethod lookupMethod;

                if (strcmp(lookupString.c_str(), "regex") == 0) {
                    lookupMethod = LookupMethod::Regex;
                } else if (strcmp(lookupString.c_str(), "exact") == 0) {
                    lookupMethod = LookupMethod::Exact;
                } else if (strcmp(lookupString.c_str(), "contains") == 0) {
                    lookupMethod = LookupMethod::Contains;
                }

                std::optional<int> dupeAmount = getIfExists<int>(gameObjectDataVal, DUPLICATIONAMOUNT);

                std::optional<bool> active = getIfExists<bool>(gameObjectDataVal, ACTIVE);
                auto scale = GetVectorData(gameObjectDataVal, SCALE);
                auto position = GetVectorData(gameObjectDataVal, POSITION);
                auto rotation = GetVectorData(gameObjectDataVal, OBJECTROTATION);
                auto localPosition = GetVectorData(gameObjectDataVal, LOCALPOSITION);
                auto localRotation = GetVectorData(gameObjectDataVal, LOCALROTATION);

                auto foundObjects = LookupId(id, lookupMethod);

                std::vector<GameObjectInfo> gameObjectInfos;

                if (dupeAmount) {
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
                                                                       gameObject->get_transform(), gameObjectInfos,
                                                                       componentDatas);
                            for (auto &o : _gameObjectInfos) {
                                if (o.GameObject == newGameObject)
                                    gameObjectInfos.push_back(o);
                            }


                        }
                    }
                } else {
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
                        }

                        if (rotation || localRotation) {
                            RingRotationOffsets[trackLaneRing] = transform->get_eulerAngles();
                        }
                    }


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

    for (int i = 0; i < gameObject->get_childCount(); i++) {
        auto child = gameObject->GetChild(i);
        children.push_back(child);
        GetChildRecursive(child, children);
    }
}


