#pragma once

#include "UnityEngine/Transform.hpp"
#include "GameObjectInfo.hpp"

namespace Chroma {

    class ComponentInitializer {
    public:
        static void InitializeComponents(UnityEngine::Transform* root, UnityEngine::Transform* original, std::vector<GameObjectInfo> gameObjectInfos);
    };

}
