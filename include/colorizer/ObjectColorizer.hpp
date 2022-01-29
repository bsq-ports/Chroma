#pragma once

#include <vector>
#include <optional>
#include <functional>

#include "UnityEngine/Color.hpp"
#include "sombrero/shared/ColorUtils.hpp"
#include "questui_components/shared/concepts.hpp"

namespace Chroma {
    template <typename T>
    concept ObjectColorizerMethods = requires(T t, std::optional<Sombrero::FastColor> const& color) {
        t.Colorize(color);
        {t.getColor()} -> QUC::IsQUCConvertible<Sombrero::FastColor>;
        {t.getSelfColor()} -> QUC::IsQUCConvertible<std::optional<Sombrero::FastColor>>;
    };

    template <typename T>
    concept ObjectColorizerInternalMethods = ObjectColorizerMethods<T> && requires(T t) {
        t.Refresh();
        {t.GlobalColorGetter()} -> QUC::IsQUCConvertible<std::optional<Sombrero::FastColor>>;
        {t.OriginalColorGetter()} -> QUC::IsQUCConvertible<std::optional<Sombrero::FastColor>>;
        {t.OriginalColor} -> QUC::IsQUCConvertible<std::optional<Sombrero::FastColor>>;
    };

    template <typename T>
    class ObjectColorizer {
    private:
        std::optional<Sombrero::FastColor> _color;
    protected:
        Sombrero::FastColor OriginalColor;

        // virtual
        [[nodiscard]] std::optional<Sombrero::FastColor> OriginalColorGetter() {
            return OriginalColor;
        }

        // abstract void Refresh()
        // abstract std::optional<Sombrero::FastColor> GlobalColorGetter()

    public:
        [[nodiscard]] std::optional<Sombrero::FastColor> getSelfColor() const {
            return _color;
        }

        Sombrero::FastColor getColor() {
            if (_color)
                return *_color;

            auto globalColor = static_cast<T*>(this)->GlobalColorGetter();
            if (globalColor)
                return *globalColor;

            // Throw an exception here intentionally or CRASH?
            return *static_cast<T*>(this)->OriginalColorGetter();
        }

        void Colorize(std::optional<Sombrero::FastColor> const& color) {
            _color = color;
            static_cast<T*>(this)->Refresh();
        }

        virtual ~ObjectColorizer() = default;
    };

}
