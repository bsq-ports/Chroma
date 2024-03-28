#include "environment_enhancements/GameObjectInfo.hpp"

#include "UnityEngine/Transform.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

#include <string>
#include <sstream>

Chroma::GameObjectInfo::GameObjectInfo(UnityEngine::GameObject* gameObject) {
  std::vector<std::string> nameList;

  UnityEngine::Transform* transform = gameObject->get_transform();
  while (true) {
    UnityEngine::GameObject* transformGameObject = transform->get_gameObject();
    UnityEngine::Transform* transformParent = transform->get_parent();
    int index = 0;
    if (transformParent != nullptr) {
      index = transform->GetSiblingIndex();
    } else {
      // Why doesnt GetSiblingIndex work on root objects?
      auto rootGameObjects = transformGameObject->get_scene().GetRootGameObjects();
      index = rootGameObjects->IndexOf(transformGameObject);
    }

    nameList.emplace_back("[" + std::to_string(index) + "]" + static_cast<std::string>(transform->get_name()));

    if (transformParent == nullptr) {
      break;
    }

    transform = transformParent;
  }

  nameList.emplace_back(gameObject->get_scene().get_name());
  std::reverse(nameList.begin(), nameList.end());

  std::stringstream id;

  // loop for each element until the second last; to avoid having "secondlast.last."
  for (int i = 0; i < nameList.size() - 1; i++) {
    id << nameList[i] + ".";
  }

  id << nameList.back();

  FullID = id.str();
  FullIDHash = std::hash<std::string_view>()(FullID);

  this->GameObject = gameObject;
}
