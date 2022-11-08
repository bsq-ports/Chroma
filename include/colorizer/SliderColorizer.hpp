#pragma once

#include "GlobalNamespace/SliderController.hpp"
#include "GlobalNamespace/ColorManager.hpp"


#include <vector>
#include <string>
#include <optional>
#include <stack>
#include <tuple>

#include "ChromaObjectData.hpp"
#include "ObjectColorizer.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"

using ColorPair = std::pair<std::optional<Sombrero::FastColor>, std::optional<Sombrero::FastColor>>;
using SliderColorStack = std::stack<ColorPair>;

namespace Chroma {
    class SliderColorizer : public ObjectColorizer<SliderColorizer>
    {
    private:
        friend class ObjectColorizer<SliderColorizer>;
        friend class NoteColorizer;

        GlobalNamespace::SliderController* _sliderController;
        std::optional<std::array<Sombrero::FastColor, 2>> _originalColors;

        SliderColorizer(GlobalNamespace::SliderController* sliderController);

    protected:
//        override Color? GlobalColorGetter => GlobalColor[(int)ColorType];
//        override Color OriginalColorGetter => OriginalColors[(int)ColorType];
        std::optional<Sombrero::FastColor> GlobalColorGetter();

        std::optional<Sombrero::FastColor> OriginalColorGetter();

        void Refresh();

    public:
        inline static bool SliderColorable = false;
        inline static UnorderedEventCallback<GlobalNamespace::SliderController*, Sombrero::FastColor const&, GlobalNamespace::ColorType> SliderColorChanged;
        inline static std::unordered_map<GlobalNamespace::SliderController const*, SliderColorizer> Colorizers;
        std::array<Sombrero::FastColor, 2> getOriginalColors();
        GlobalNamespace::ColorType getColorType();

        friend class std::pair<GlobalNamespace::SliderController const*, SliderColorizer>;
        friend class std::pair<const GlobalNamespace::SliderController *const, Chroma::SliderColorizer>;
        SliderColorizer(SliderColorizer const&) = delete;
        static SliderColorizer* New(GlobalNamespace::SliderController* sliderControllerBase);

        static void Reset();

        // extensions
        inline static SliderColorizer* GetSliderColorizer(GlobalNamespace::SliderController* sliderController) {
            auto it = Colorizers.find(sliderController);
            if (it == Colorizers.end())
                return nullptr;

            return &it->second;
        }

        inline static void ColorizeSlider(GlobalNamespace::SliderController* sliderController, std::optional<Sombrero::FastColor> const& color) {
            auto colorizer = GetSliderColorizer(sliderController);
            if (colorizer)
                colorizer->Colorize(color);
        }
    };
}

