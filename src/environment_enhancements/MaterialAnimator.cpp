#include "environment_enhancements/MaterialAnimator.hpp"
#include "sombrero/shared/FastColor.hpp"
#include "AnimationHelper.hpp"

DEFINE_TYPE(Chroma, MaterialAnimator)

using namespace Chroma;

void Chroma::MaterialAnimator::Update() {
  for (auto& materialInfo : materials) {
    bool updated = 0;
    std::optional<Sombrero::FastColor> colorOffset = AnimationHelper::GetColorOffset(
        std::nullopt, *materialInfo.Track, 0, updated, context->GetBaseProviderContext(), lastCheckedTime);

    if (colorOffset) {
      materialInfo.Material->set_color(*colorOffset);
    }
  }
  lastCheckedTime = Animation::getCurrentTime();
}
