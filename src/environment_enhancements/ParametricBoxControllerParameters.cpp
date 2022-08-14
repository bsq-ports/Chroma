#include "environment_enhancements/ParametricBoxControllerParameters.hpp"

using namespace GlobalNamespace;
using namespace UnityEngine;

void Chroma::ParametricBoxControllerParameters::SetTransformScale(ParametricBoxController* parametricBoxController,
                                                                  const Sombrero::FastVector3 &scale) {
    GetParameters(parametricBoxController).Scale = scale;
}

void Chroma::ParametricBoxControllerParameters::SetTransformPosition(
        GlobalNamespace::ParametricBoxController *parametricBoxController, const Sombrero::FastVector3 &position) {
    GetParameters(parametricBoxController).Position = position;
}

Chroma::ParametricBoxControllerParameters& Chroma::ParametricBoxControllerParameters::GetParameters(
        GlobalNamespace::ParametricBoxController *parametricBoxController) {
    return TransformParameters[parametricBoxController];
}
