#pragma once

#include <concepts>
#include <optional>

#include "sombrero/shared/ColorUtils.hpp"

namespace Chroma {

template <typename T, typename U>
concept IsConvertible = std::is_convertible_v<T, U>;

template <typename T>
concept ObjectColorizerMethods = requires(T t, std::optional<Sombrero::FastColor> const& color) {
  t.Colorize(color);
  { t.getColor() } -> IsConvertible<Sombrero::FastColor>;
  { t.getSelfColor() } -> IsConvertible<std::optional<Sombrero::FastColor>>;
};

template <typename T>
concept ObjectColorizerInternalMethods = ObjectColorizerMethods<T> && requires(T t) {
  t.Refresh();
  { t.GlobalColorGetter() } -> IsConvertible<std::optional<Sombrero::FastColor>>;
  { t.OriginalColorGetter() } -> IsConvertible<std::optional<Sombrero::FastColor>>;
  { t.OriginalColor } -> IsConvertible<std::optional<Sombrero::FastColor>>;
};

class ObjectColorizer {
private:
  std::optional<Sombrero::FastColor> _color;

protected:
  Sombrero::FastColor OriginalColor;

  [[nodiscard]] virtual std::optional<Sombrero::FastColor> getGlobalColor() const = 0;

  [[nodiscard]] virtual Sombrero::FastColor getOriginalColor() const {
    return OriginalColor;
  }

public:  
  ObjectColorizer() = default;
  ObjectColorizer(std::optional<Sombrero::FastColor> color) : _color(color) {}

  [[nodiscard]] virtual Sombrero::FastColor getColor() const {
    return _color.value_or(getGlobalColor().value_or(getOriginalColor()));
  }


  constexpr void Colorize(std::optional<Sombrero::FastColor> const& color) {
    _color = color;
    Refresh();
  }

  virtual void Refresh() = 0;
};

} // namespace Chroma
