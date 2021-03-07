#pragma once

#include "UnityEngine/Color.hpp"

namespace Chroma {

    inline static int floorToInt(float f) {
        return (int) std::floor((double) f);
    }

    // Stolen from Caeden117's ChroMapper code
    // https://github.com/Caeden117/ChroMapper/blob/eb62b8d732b5dc0e60002de940ec0b7d41f3f12c/Assets/__Scripts/ColourManager.cs
    class ColourManager {
        /*
         * ColourManager Lite™ | Slimmed down version from Chroma
         */

    public:
        inline static int ColourToInt(float rC, float gC, float bC, float aC) {
            int r = floorToInt(rC * 255);
            int g = floorToInt(gC * 255);
            int b = floorToInt(bC * 255);
            int a = floorToInt(aC * 255);
            return (((a & 0x0ff) << 24) | ((r & 0x0ff) << 16) | ((g & 0x0ff) << 8) | (b & 0x0ff));
        }

        inline static int ColourToInt(UnityEngine::Color color) {
            return ColourToInt(color.r, color.g, color.b, color.a);
        }

        inline static UnityEngine::Color ColourFromInt(int rgb) {
            int alpha = (rgb >> 24) & 0x0ff;
            int red = (rgb >> 16) & 0x0ff;
            int green = (rgb >> 8) & 0x0ff;
            int blue = (rgb) & 0x0ff;
            return UnityEngine::Color((float) red / 255.0f, (float) green / 255.0f, (float) blue / 255.0f,
                                      (float) alpha / 255.0f);
        }
    };

    /// These color functions convert color to a single int so we can use it in extern functions
    /// Since extern functions can't return UnityEngine::Color

    inline float linearToGamma(float c) {
        return 255.0f * std::pow(c / 255.0f, 1 / 2.2f);
    }

    inline float gammaToLinear(float c) {
        return 255.0f * std::pow(c / 255.0f, 2.2f);
    }
}