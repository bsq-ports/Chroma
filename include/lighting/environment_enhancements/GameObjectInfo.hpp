#pragma once

#include "UnityEngine/GameObject.hpp"

namespace Chroma {

    class GameObjectInfo {
    public:
        std::string FullID;
        UnityEngine::GameObject* GameObject;

        explicit GameObjectInfo(UnityEngine::GameObject* gameObject);
    };

}

