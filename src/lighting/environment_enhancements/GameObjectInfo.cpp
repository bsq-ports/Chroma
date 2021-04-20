#include "lighting/environment_enhancements/GameObjectInfo.hpp"

#include "UnityEngine/Transform.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

Chroma::GameObjectInfo::GameObjectInfo(UnityEngine::GameObject *gameObject) {
    std::vector<std::string> nameList;

    UnityEngine::Transform* transform = gameObject->get_transform();
    while (true)
    {
        int index;
        if (transform->get_parent() != nullptr)
        {
            index = transform->GetSiblingIndex();
        }
        else
        {
            // Why doesnt GetSiblingIndex work on root objects?
            auto rootGameObjects = transform->get_gameObject()->get_scene().GetRootGameObjects();
            index = rootGameObjects->IndexOf(transform->get_gameObject());
        }

        nameList.push_back("[" + std::to_string(index) + "]" + to_utf8(csstrtostr(transform->get_name())));

        if (transform->get_parent() == nullptr)
        {
            break;
        }

        transform = transform->get_parent();
    }

    nameList.push_back(to_utf8(csstrtostr(gameObject->get_scene().get_name())));
    std::reverse(nameList.begin(), nameList.end());

    std::string id;

    for (int i = 0; i < nameList.size() - 1; i++) {
        id += nameList[i] + ".";
    }

    id += nameList.back();

    FullID = std::string(id);

    this->GameObject = gameObject;
}
