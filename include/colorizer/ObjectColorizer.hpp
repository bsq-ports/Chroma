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
        virtual std::optional<Sombrero::FastColor> OriginalColorGetter();

        virtual void Refresh() = 0;

    public:
        std::optional<Sombrero::FastColor> getSelfColor();

        virtual Sombrero::FastColor getColor();

        void Colorize(std::optional<Sombrero::FastColor> const& color);
    };

}
