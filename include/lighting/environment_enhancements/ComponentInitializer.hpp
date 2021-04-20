#pragma once

#include "UnityEngine/Transform.hpp"

namespace Chroma {

    class ComponentInitializer {
    public:
        static void InitializeComponents(UnityEngine::Transform* root, UnityEngine::Transform* original);
    };

}
