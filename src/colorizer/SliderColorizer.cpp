#include "Chroma.hpp"

#include <unordered_map>
#include "colorizer/SliderColorizer.hpp"
#include "ChromaController.hpp"
#include "utils/ChromaUtils.hpp"
#include "ChromaObjectData.hpp"


using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Sombrero;
using namespace Chroma;

SliderColorizer::SliderColorizer(GlobalNamespace::SliderController *sliderController)
:_sliderController(sliderController)
{
    CRASH_UNLESS(_sliderController);
}

SliderColorizer* SliderColorizer::New(GlobalNamespace::SliderController *sliderControllerBase) {
    if (!ChromaController::DoColorizerSabers())
        return nullptr;

    return &Colorizers.try_emplace(sliderControllerBase, sliderControllerBase).first->second;
}

std::array<Sombrero::FastColor, 2> SliderColorizer::getOriginalColors() {
    if (!_originalColors) {
        ColorManager *colorManager = _sliderController->colorManager;
        if (colorManager) {
            _originalColors =
                    {
                            colorManager->ColorForType(ColorType::ColorA),
                            colorManager->ColorForType(ColorType::ColorB),
                    };
        } else {
            getLogger().warning("_colorManager was null, defaulting to red/blue");
            _originalColors =
                    {
                            Sombrero::FastColor(0.784f, 0.078f, 0.078f),
                            Sombrero::FastColor(0, 0.463f, 0.823f),
                    };
        }
    }


    return *_originalColors;
}

GlobalNamespace::ColorType SliderColorizer::getColorType() {
    if (_sliderController && _sliderController->sliderData) {
        auto sliderData = _sliderController->sliderData;

        return sliderData->colorType;
    }

    return ColorType::ColorA;
}

std::optional<Sombrero::FastColor> SliderColorizer::GlobalColorGetter() {
    return GlobalColor[(int) getColorType()];
}

std::optional<Sombrero::FastColor> SliderColorizer::OriginalColorGetter() {
    return getOriginalColors()[(int) getColorType()];
}

void SliderColorizer::Reset() {
    GlobalColor[0] = std::nullopt;
    GlobalColor[1] = std::nullopt;
    Colorizers.clear();
    SliderColorChanged.clear();
}

void SliderColorizer::Refresh() {
    _sliderController->initColor = getColor();
}
