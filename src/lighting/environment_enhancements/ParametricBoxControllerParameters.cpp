#include "lighting/environment_enhancements/ParametricBoxControllerParameters.hpp"

using namespace GlobalNamespace;
using namespace UnityEngine;

void Chroma::ParametricBoxControllerParameters::SetTransformScale(ParametricBoxController* parametricBoxController,
                                                                  Vector3 scale) {
    GetParameters(parametricBoxController)->Scale = scale;
}

void Chroma::ParametricBoxControllerParameters::SetTransformPosition(
        GlobalNamespace::ParametricBoxController *parametricBoxController, UnityEngine::Vector3 position) {
    GetParameters(parametricBoxController)->Position = position;
}

std::optional<Chroma::ParametricBoxControllerParameters> Chroma::ParametricBoxControllerParameters::GetParameters(
        GlobalNamespace::ParametricBoxController *parametricBoxController) {

    auto it = TransformParameters.find(parametricBoxController);

    if (it != TransformParameters.end()) {
        auto parameters = it->second;
        TransformParameters[parametricBoxController] = parameters;

        return parameters;
    }


    return std::nullopt;
}
