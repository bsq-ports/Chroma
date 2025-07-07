#pragma once

#include "ObjectColorizer.hpp"

#include "GlobalNamespace/ColorType.hpp"

#include <optional>

namespace GlobalNamespace {
class SliderController;
class ColorSO;
class MultipliedColorSO;
} // namespace GlobalNamespace

using ColorPair = std::pair<std::optional<Sombrero::FastColor>, std::optional<Sombrero::FastColor>>;
using SliderColorStack = std::stack<ColorPair>;

namespace Chroma {
class SliderColorizer : public ObjectColorizer {
friend class std::pair<GlobalNamespace::SliderController const*, SliderColorizer>;
friend class std::pair<GlobalNamespace::SliderController const* const, SliderColorizer>;

private:
  GlobalNamespace::SliderController* _sliderController;

  SliderColorizer(GlobalNamespace::SliderController* sliderController);

protected:
  //        override Color? GlobalColorGetter => GlobalColor[(int)ColorType];
  //        override Color OriginalColorGetter => OriginalColors[(int)ColorType];
  [[nodiscard]] std::optional<Sombrero::FastColor> getGlobalColor() const final;

  [[nodiscard]] Sombrero::FastColor getOriginalColor() const final;


public:
  inline static bool SliderColorable = false;
  inline static UnorderedEventCallback<GlobalNamespace::SliderController*, Sombrero::FastColor const&,
                                       GlobalNamespace::ColorType>
      SliderColorChanged;
  inline static std::unordered_map<GlobalNamespace::SliderController const*, SliderColorizer> Colorizers;

  SliderColorizer(SliderColorizer const&) = delete;
  static SliderColorizer* New(GlobalNamespace::SliderController* sliderControllerBase);

  static void Reset();

  [[nodiscard]] GlobalNamespace::ColorType getColorType() const;
  void Refresh() final;

  // extensions
  static SliderColorizer* GetSliderColorizer(GlobalNamespace::SliderController* sliderController) {
    auto it = Colorizers.find(sliderController);
    if (it == Colorizers.end()) {
      return nullptr;
    }

    return &it->second;
  }

  constexpr static void ColorizeSlider(GlobalNamespace::SliderController* sliderController,
                             std::optional<Sombrero::FastColor> const& color) {
    auto* colorizer = GetSliderColorizer(sliderController);
    if (colorizer) {
      colorizer->Colorize(color);
    }
  }
};
} // namespace Chroma
