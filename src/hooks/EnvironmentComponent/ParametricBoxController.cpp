#include "Chroma.hpp"
#include "ChromaController.hpp"
#include "environment_enhancements/ParametricBoxControllerParameters.hpp"
#include "colorizer/ObstacleColorizer.hpp"
#include "utils/ChromaUtils.hpp"
#include "ChromaObjectData.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"

#include "GlobalNamespace/ColorManager.hpp"
#include "GlobalNamespace/ObstacleController.hpp"
#include "GlobalNamespace/ParametricBoxController.hpp"

#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/MaterialPropertyBlock.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/GameObject.hpp"


using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace Chroma;
using namespace ChromaUtils;

Sombrero::FastVector3 const& GetTransformScale(Sombrero::FastVector3 const& defaultV,
                                               ParametricBoxControllerParameters const& parameters) {

  if (parameters.Scale) {
    return parameters.Scale.value();
  }

  return defaultV;
}

Sombrero::FastVector3 const& GetTransformPosition(Sombrero::FastVector3 const& defaultV,
                                                  ParametricBoxControllerParameters const& parameters) {

  if (parameters.Position) {
    return parameters.Position.value();
  }

  return defaultV;
}

MAKE_HOOK_MATCH(ParametricBoxController_Refresh, &ParametricBoxController::Refresh, void,
                ParametricBoxController* self) {
  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    ParametricBoxController_Refresh(self);
    return;
  }

  if (self->____meshRenderer == nullptr) {
    return;
  }

  auto scale = Sombrero::FastVector3(self->width * 0.5F, self->height * 0.5F, self->length * 0.5F);
  auto pos = Sombrero::FastVector3(0.0F, (0.5F - self->heightCenter) * self->height, 0.0F);

  auto it = ParametricBoxControllerParameters::TransformParameters.find(self);

  if (it != ParametricBoxControllerParameters::TransformParameters.end()) {
    scale = GetTransformScale(scale, it->second);
    pos = GetTransformPosition(pos, it->second);
  }

  auto transform = self->get_transform();

  transform->set_localScale(scale);
  transform->set_localPosition(pos);

  static auto* materialPropertyBlock = ParametricBoxController::getStaticF__materialPropertyBlock();

  if (materialPropertyBlock == nullptr) {
    materialPropertyBlock = UnityEngine::MaterialPropertyBlock::New_ctor();
    ParametricBoxController::setStaticF__materialPropertyBlock(materialPropertyBlock);
  }

  auto color = self->color;
  color.a *= self->alphaMultiplier;
  if (color.a < self->minAlpha) {
    color.a = self->minAlpha;
  }

  static auto colorId = ParametricBoxController::getStaticF__colorID();
  static auto alphaStartID = ParametricBoxController::getStaticF__alphaStartID();
  static auto alphaEndID = ParametricBoxController::getStaticF__alphaEndID();
  static auto widthStartID = ParametricBoxController::getStaticF__widthStartID();
  static auto widthEndID = ParametricBoxController::getStaticF__widthEndID();

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

  static auto SetColor = FPtrWrapper<static_cast<void (UnityEngine::MaterialPropertyBlock::*)(int, UnityEngine::Color)>(
      &UnityEngine::MaterialPropertyBlock::SetColor)>::get();
  static auto SetFloat = FPtrWrapper<static_cast<void (UnityEngine::MaterialPropertyBlock::*)(int, float)>(
      &UnityEngine::MaterialPropertyBlock::SetFloat)>::get();
  static auto SetPropertyBlock =
      FPtrWrapper<static_cast<void (UnityEngine::Renderer::*)(UnityEngine::MaterialPropertyBlock*)>(
          &UnityEngine::Renderer::SetPropertyBlock)>::get();

  SetColor(materialPropertyBlock, colorId, color);
  SetFloat(materialPropertyBlock, alphaStartID, self->alphaStart);
  SetFloat(materialPropertyBlock, alphaEndID, self->alphaEnd);
  SetFloat(materialPropertyBlock, widthStartID, self->widthStart);
  SetFloat(materialPropertyBlock, widthEndID, self->widthEnd);

  SetPropertyBlock(self->_meshRenderer, materialPropertyBlock);
  //    static auto SetPropertyBlock = il2cpp_utils::FindMethodUnsafe(classof(UnityEngine::MeshRenderer*),
  //    "SetPropertyBlock", 1);

  //    il2cpp_utils::RunMethodThrow<void>(self->meshRenderer, SetPropertyBlock, materialPropertyBlock);

  //    self->meshRenderer->SetPropertyBlock(materialPropertyBlock);
}

void ParametricBoxControllerHook() {
  INSTALL_HOOK_ORIG(ChromaLogger::Logger, ParametricBoxController_Refresh);
}

ChromaInstallHooks(ParametricBoxControllerHook)