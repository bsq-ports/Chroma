#include "environment_enhancements/MaterialAnimator.hpp"
#include "environment_enhancements/MaterialsManager.hpp"
#include "sombrero/shared/FastColor.hpp"
#include "AnimationHelper.hpp"

DEFINE_TYPE(Chroma, MaterialAnimator)

using namespace Chroma;

void Chroma::MaterialAnimator::Update() {
  for (auto& materialInfo : materials) {
    bool updated = false;
    std::optional<Sombrero::FastColor> colorOffset = AnimationHelper::GetColorOffset(
        std::nullopt, *materialInfo.Track, 0, updated, context->GetBaseProviderContext(), lastCheckedTime);

    if (!colorOffset) {
      continue;
    }

    // Post 1.39.1 fix
    // https://github.com/Aeroluna/Heck/commit/51eee7e973d8b3fe90a2a7582c54a82eb908e561
    if ((materialInfo.ShaderType == ShaderType::Standard || materialInfo.ShaderType == ShaderType::BTSPillar) &&
        materialInfo.Material->get_shaderKeywords().size() == 0) {
      *colorOffset = colorOffset->Alpha(0);
    }

    materialInfo.Material->set_color(*colorOffset);
  }
  lastCheckedTime = Animation::getCurrentTime();
}
