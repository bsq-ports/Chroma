#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "lighting/environment_enhancements/ParametricBoxControllerParameters.hpp"

#include "colorizer/ObstacleColorizer.hpp"
#include "GlobalNamespace/ColorManager.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/ObstacleController.hpp"
#include "GlobalNamespace/ParametricBoxController.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/MaterialPropertyBlock.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/GameObject.hpp"
#include "utils/ChromaUtils.hpp"

#include "ChromaObjectData.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace Chroma;
using namespace ChromaUtils;


UnityEngine::Vector3& GetTransformScale(UnityEngine::Vector3& defaultV, ParametricBoxControllerParameters& parameters) {

    if (parameters.Position) {
        return parameters.Scale.value();
    }

    return defaultV;
}

UnityEngine::Vector3& GetTransformPosition(UnityEngine::Vector3& defaultV, ParametricBoxControllerParameters& parameters) {

    if (parameters.Position) {
        return parameters.Position.value();
    }

    return defaultV;
}

MAKE_HOOK_MATCH(ParametricBoxController_Refresh,&ParametricBoxController::Refresh, void, ParametricBoxController* self) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        ParametricBoxController_Refresh(self);
        return;
    }

    if (self->meshRenderer == nullptr)
    {
        return;
    }

    auto scale = UnityEngine::Vector3(self->width * 0.5f, self->height * 0.5f, self->length * 0.5f);
    auto pos = UnityEngine::Vector3(0.0f, (0.5f - self->heightCenter) * self->height, 0.0f);

    auto it = ParametricBoxControllerParameters::TransformParameters.find(self);

    if (it != ParametricBoxControllerParameters::TransformParameters.end()) {
        scale = GetTransformScale(scale, it->second);
        pos = GetTransformPosition(pos, it->second);
    }

    static auto transformProp = il2cpp_utils::FindProperty(classof(UnityEngine::Component*), "transform");

//        auto transform = self->get_transform();

    auto transform = CRASH_UNLESS(il2cpp_utils::GetPropertyValue<UnityEngine::Transform*>(self, transformProp));

    static auto localScaleProp = il2cpp_utils::FindProperty(classof(UnityEngine::Transform*), "localScale");
    static auto localPosProp = il2cpp_utils::FindProperty(classof(UnityEngine::Transform*), "localPosition");

    CRASH_UNLESS(il2cpp_utils::SetPropertyValue(transform, localScaleProp, scale));
    CRASH_UNLESS(il2cpp_utils::SetPropertyValue(transform, localPosProp, pos));

    static auto materialPropertyBlock = ParametricBoxController::_get__materialPropertyBlock();

    if (materialPropertyBlock == nullptr) {
        materialPropertyBlock = UnityEngine::MaterialPropertyBlock::New_ctor();
        ParametricBoxController::_set__materialPropertyBlock(materialPropertyBlock);
    }


    auto color = self->color;
    color.a *= self->alphaMultiplier;
    if (color.a < self->minAlpha)
    {
        color.a = self->minAlpha;
    }

    static auto colorId = ParametricBoxController::_get__colorID();
    static auto alphaStartID = ParametricBoxController::_get__alphaStartID();
    static auto alphaEndID = ParametricBoxController::_get__alphaEndID();
    static auto widthStartID = ParametricBoxController::_get__widthStartID();
    static auto widthEndID = ParametricBoxController::_get__widthEndID();

//    static auto SetColor = il2cpp_utils::FindMethodUnsafe(materialPropertyBlock, "SetColor", 2);
//    static auto SetFloat = il2cpp_utils::FindMethodUnsafe(materialPropertyBlock, "SetFloat", 2);
//
//
//    CRASH_UNLESS(materialPropertyBlock);
//    CRASH_UNLESS(SetColor);
//
//    il2cpp_utils::RunMethodThrow<void>(materialPropertyBlock, SetColor, colorId, color);
//    il2cpp_utils::RunMethodThrow<void>(materialPropertyBlock, SetFloat, alphaStartID, self->alphaStart);
//    il2cpp_utils::RunMethodThrow<void>(materialPropertyBlock, SetFloat, alphaEndID, self->alphaEnd);
//    il2cpp_utils::RunMethodThrow<void>(materialPropertyBlock, SetFloat, widthStartID, self->widthStart);
//    il2cpp_utils::RunMethodThrow<void>(materialPropertyBlock, SetFloat, widthEndID, self->widthEnd);


    static auto SetColor = FPtrWrapper<static_cast<void (UnityEngine::MaterialPropertyBlock::*)(int, UnityEngine::Color)>(&UnityEngine::MaterialPropertyBlock::SetColor)>::get();
    static auto SetFloat = FPtrWrapper<static_cast<void (UnityEngine::MaterialPropertyBlock::*)(int, float)>(&UnityEngine::MaterialPropertyBlock::SetFloat)>::get();
    static auto SetPropertyBlock = FPtrWrapper<static_cast<void (UnityEngine::Renderer::*)(UnityEngine::MaterialPropertyBlock*)>(&UnityEngine::Renderer::SetPropertyBlock)>::get();



    SetColor(materialPropertyBlock, colorId, color);
    SetFloat(materialPropertyBlock, alphaStartID, self->alphaStart);
    SetFloat(materialPropertyBlock, alphaEndID, self->alphaEnd);
    SetFloat(materialPropertyBlock, widthStartID, self->widthStart);
    SetFloat(materialPropertyBlock, widthEndID, self->widthEnd);

    SetPropertyBlock(self->meshRenderer, materialPropertyBlock);
//    static auto SetPropertyBlock = il2cpp_utils::FindMethodUnsafe(classof(UnityEngine::MeshRenderer*), "SetPropertyBlock", 1);

//    il2cpp_utils::RunMethodThrow<void>(self->meshRenderer, SetPropertyBlock, materialPropertyBlock);

//    self->meshRenderer->SetPropertyBlock(materialPropertyBlock);
}

void ParametricBoxControllerHook(Logger& logger) {
    INSTALL_HOOK_ORIG(logger, ParametricBoxController_Refresh);
}

ChromaInstallHooks(ParametricBoxControllerHook)