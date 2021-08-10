#include "lighting/environment_enhancements/ParametricBoxControllerParameters.hpp"

using namespace GlobalNamespace;
using namespace UnityEngine;

void Chroma::ParametricBoxControllerParameters::SetTransformScale(ParametricBoxController* parametricBoxController,
                                                                  Sombrero::FastVector3 scale) {
    GetParameters(parametricBoxController).Scale = scale;
}

void Chroma::ParametricBoxControllerParameters::SetTransformPosition(
        GlobalNamespace::ParametricBoxController *parametricBoxController, Sombrero::FastVector3 position) {
    GetParameters(parametricBoxController).Position = position;
}

Chroma::ParametricBoxControllerParameters& Chroma::ParametricBoxControllerParameters::GetParameters(
        GlobalNamespace::ParametricBoxController *parametricBoxController) {

    auto it = TransformParameters.find(parametricBoxController);

    if (it != TransformParameters.end()) {
        return it->second;
    } else {
        auto it2 = TransformParameters.emplace(parametricBoxController, ParametricBoxControllerParameters());

        return TransformParameters[parametricBoxController];
    }
}
