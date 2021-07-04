#pragma once

#include <vector>
#include <optional>
#include <functional>

#include <UnityEngine/Color.hpp>

namespace Chroma {
    class ObjectColorizer {
    private:
        std::optional<UnityEngine::Color> _color;
    protected:
        UnityEngine::Color OriginalColor;
        virtual std::optional<UnityEngine::Color> GlobalColorGetter() = 0;
        virtual std::optional<UnityEngine::Color> OriginalColorGetter();

        virtual void Refresh() = 0;

    public:
        std::optional<UnityEngine::Color> getSelfColor();

        virtual UnityEngine::Color getColor();

        void Colorize(std::optional<UnityEngine::Color> color);
    };

}
