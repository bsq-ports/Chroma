#include "SaberAPI.hpp"

#include <utility>
#include "colorizer/SaberColorizer.hpp"

#include "conditional-dependencies/shared/main.hpp"
#include "shared/utils.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;



EXPOSE_API(getGlobalSaberColorSafe, OptColor, int saberType) {
    CRASH_UNLESS(saberType >= SaberType::SaberA && saberType <= SaberType::SaberB);

    auto optional = SaberColorizer::GlobalColor[saberType];

    UnityEngine::Color color;

    if (optional) {

        color = optional.value();

        return OptColorFromColor(color);
    } else {
        return OptColorNull;
    }
}

EXPOSE_API(setGlobalSaberColorSafe, void, int saberType, std::optional<UnityEngine::Color> color) {
    SaberColorizer::GlobalColorize(saberType, color);
}

EXPOSE_API(getSaberColorSafe, OptColor, GlobalNamespace::SaberModelController* saberModelController) {
    auto colorizer = SaberColorizer::GetColorizer(saberModelController);

    if (!colorizer)
        return OptColorNull;

    auto optional = colorizer->getSelfColor();

    UnityEngine::Color color;

    if (optional) {
        color = optional.value();
        return OptColorFromColor(color);
    } else {
        return OptColorNull;
    }
}

EXPOSE_API(setSaberColorSafe, void, GlobalNamespace::SaberModelController* saberModelController, std::optional<UnityEngine::Color> color) {
    SaberColorizer::ColorizeSaber(saberModelController, color);
}

EXPOSE_API(getGlobalSabersColorSafe, SaberAPI::ColorOptPair) {
    auto colorA = SaberColorizer::GlobalColor[SaberType::SaberA];
    auto colorB = SaberColorizer::GlobalColor[SaberType::SaberB];

    return SaberAPI::ColorOptPair {OptColorFromColor(colorA), OptColorFromColor(colorB)};
}

extern "C" UnorderedEventCallback<int, GlobalNamespace::SaberModelController*, UnityEngine::Color>* __getSaberChangedColorCallbackSafe() {
    return &SaberColorizer::SaberColorChanged;
}

extern "C" void __registerSaberCallbackSafe(const ThinVirtualLayer<void (void*, int, GlobalNamespace::SaberModelController*, UnityEngine::Color)>& callback) {
    SaberColorizer::SaberColorChanged += callback;
}

extern "C" void __setSaberColorable(GlobalNamespace::SaberModelController* saberModelController, bool colorable) {
    SaberColorizer::SetColorable(saberModelController, colorable);
}

extern "C" bool __isSaberColorable(GlobalNamespace::SaberModelController* saberModelController) {
    return SaberColorizer::IsColorable(saberModelController);
}