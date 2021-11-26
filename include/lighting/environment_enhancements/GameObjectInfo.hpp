#pragma once

#include "UnityEngine/GameObject.hpp"

namespace Chroma {

    class GameObjectInfo {
    public:
        std::string FullID;
        size_t FullIDHash; // for faster lookups
        UnityEngine::GameObject* GameObject;

        explicit GameObjectInfo(UnityEngine::GameObject* gameObject);
    };

}

