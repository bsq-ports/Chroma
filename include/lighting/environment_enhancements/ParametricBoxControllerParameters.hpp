#pragma once

#include "GlobalNamespace/ParametricBoxController.hpp"

#include "UnityEngine/Vector3.hpp"

#include <unordered_map>
#include <optional>

namespace Chroma {
    class ParametricBoxControllerParameters {
    public:
        inline static std::unordered_map<GlobalNamespace::ParametricBoxController*, ParametricBoxControllerParameters> TransformParameters;

        std::optional<UnityEngine::Vector3> Scale;
        std::optional<UnityEngine::Vector3> Position;

        static void SetTransformScale(GlobalNamespace::ParametricBoxController* parametricBoxController, UnityEngine::Vector3 scale);

        static void SetTransformPosition(GlobalNamespace::ParametricBoxController* parametricBoxController, UnityEngine::Vector3 position);

    private:
        static std::optional<ParametricBoxControllerParameters> GetParameters(GlobalNamespace::ParametricBoxController* parametricBoxController);
    };
}