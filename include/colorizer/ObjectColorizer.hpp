#pragma once

#include <vector>
#include <optional>
#include <functional>

#include "UnityEngine/Color.hpp"
#include "sombrero/shared/ColorUtils.hpp"

namespace Chroma {
    class ObjectColorizer {
    private:
        std::optional<Sombrero::FastColor> _color;
    protected:
        Sombrero::FastColor OriginalColor;
        virtual std::optional<Sombrero::FastColor> GlobalColorGetter() = 0;
        virtual std::optional<Sombrero::FastColor> OriginalColorGetter() {
            return OriginalColor;
        }

        virtual void Refresh() = 0;

    public:
        std::optional<Sombrero::FastColor> getSelfColor() {
            return _color;
        }

        virtual Sombrero::FastColor getColor() {
            if (_color)
                return *_color;

            auto globalColor = GlobalColorGetter();
            if (globalColor)
                return *globalColor;

            // Throw an exception here intentionally or CRASH?
            return *OriginalColorGetter();
        }

        void Colorize(std::optional<Sombrero::FastColor> const& color) {
            _color = color;
            Refresh();
        }

        virtual ~ObjectColorizer() = default;
    };

}
