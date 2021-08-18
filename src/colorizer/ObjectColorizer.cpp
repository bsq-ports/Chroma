#include "colorizer/ObjectColorizer.hpp"

using namespace Sombrero;

Sombrero::FastColor Chroma::ObjectColorizer::getColor() {
    if (_color)
        return *_color;

    auto globalColor = GlobalColorGetter();
    if (globalColor)
        return *globalColor;

    // Throw an exception here intentionally or CRASH?
    return *OriginalColorGetter();
}

void Chroma::ObjectColorizer::Colorize(std::optional<Sombrero::FastColor> const& color) {
    _color = color;
    Refresh();
}

std::optional<Sombrero::FastColor> Chroma::ObjectColorizer::OriginalColorGetter() {
    return OriginalColor;
}

std::optional<Sombrero::FastColor> Chroma::ObjectColorizer::getSelfColor() {
    return _color;
}
