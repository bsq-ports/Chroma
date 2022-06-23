#pragma once

#include "GlobalNamespace/ParametricBoxController.hpp"

#include "UnityEngine/Vector3.hpp"
#include "sombrero/shared/Vector3Utils.hpp"

#include <unordered_map>
#include <optional>

namespace Chroma {
    class ParametricBoxControllerParameters {
    public:
        inline static std::unordered_map<GlobalNamespace::ParametricBoxController*, ParametricBoxControllerParameters> TransformParameters;

        std::optional<Sombrero::FastVector3> Scale;
        std::optional<Sombrero::FastVector3> Position;

        static void SetTransformScale(GlobalNamespace::ParametricBoxController* parametricBoxController, const Sombrero::FastVector3 &scale);

        static void SetTransformPosition(GlobalNamespace::ParametricBoxController* parametricBoxController, const Sombrero::FastVector3 &position);

    private:
        static ParametricBoxControllerParameters& GetParameters(GlobalNamespace::ParametricBoxController* parametricBoxController);
    };
}