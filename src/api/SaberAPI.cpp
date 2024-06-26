#include "SaberAPI.hpp"

#include <utility>
#include "colorizer/SaberColorizer.hpp"

#include "conditional-dependencies/shared/main.hpp"
#include "shared/utils.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Sombrero;

EXPOSE_API(getGlobalSaberColorSafe, OptColor, int saberType) {
  CRASH_UNLESS(saberType >= SaberType::SaberA.value__ && saberType <= SaberType::SaberB.value__);

  auto optional = SaberColorizer::GlobalColor[saberType];

  Sombrero::FastColor color;

  if (optional) {

    color = optional.value();

    return OptColorFromColor(color);
  }
  return OptColorNull;
}

EXPOSE_API(setGlobalSaberColorSafe, void, int saberType, std::optional<Sombrero::FastColor> color) {
  SaberColorizer::GlobalColorize(saberType, color);
}

EXPOSE_API(getSaberColorSafe, OptColor, GlobalNamespace::SaberModelController* saberModelController) {
  auto& colorizer = SaberColorizer::GetColorizer(saberModelController);

  auto optional = colorizer.getSelfColor();

  Sombrero::FastColor color;

  if (optional) {
    color = optional.value();
    return OptColorFromColor(color);
  }
  return OptColorNull;
}

EXPOSE_API(setSaberColorSafe, void, GlobalNamespace::SaberModelController* saberModelController,
           std::optional<Sombrero::FastColor> color) {
  SaberColorizer::ColorizeSaber(saberModelController, color);
}

EXPOSE_API(getGlobalSabersColorSafe, SaberAPI::ColorOptPair) {
  auto colorA = SaberColorizer::GlobalColor[SaberType::SaberA.value__];
  auto colorB = SaberColorizer::GlobalColor[SaberType::SaberB.value__];

  return SaberAPI::ColorOptPair{ OptColorFromColor(colorA), OptColorFromColor(colorB) };
}

EXPOSE_API(getSaberChangedColorCallbackSafe, SaberAPI::SaberCallback*) {
  return &SaberColorizer::SaberColorChanged;
}

EXPOSE_API(registerSaberCallbackSafe, void, SaberAPI::SaberCallbackFunc const& callback) {
  SaberColorizer::SaberColorChanged += callback;
}

EXPOSE_API(setSaberColorable, void, GlobalNamespace::SaberModelController* saberModelController, bool colorable) {
  SaberColorizer::SetColorable(saberModelController, colorable);
}

EXPOSE_API(isSaberColorable, bool, GlobalNamespace::SaberModelController* saberModelController) {
  return SaberColorizer::IsColorable(saberModelController);
}