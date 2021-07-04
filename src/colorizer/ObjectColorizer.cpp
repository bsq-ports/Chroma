#include "colorizer/ObjectColorizer.hpp"

UnityEngine::Color Chroma::ObjectColorizer::getColor() {
    if (_color)
        return *_color;

    auto globalColor = GlobalColorGetter();
    if (globalColor)
        return *globalColor;

    // Throw an exception here intentionally or CRASH?
    return *OriginalColorGetter();
}

void Chroma::ObjectColorizer::Colorize(std::optional<UnityEngine::Color> color) {
    _color = color;
    Refresh();
}

std::optional<UnityEngine::Color> Chroma::ObjectColorizer::OriginalColorGetter() {
    return OriginalColor;
}

std::optional<UnityEngine::Color> Chroma::ObjectColorizer::getSelfColor() {
    return _color;
}
