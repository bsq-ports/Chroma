#pragma once

#include "UnityEngine/Color.hpp"

#define CHROMA_ID "chroma"

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
        // TODO: Remove
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

    template<typename T, typename V>
    struct ExternPair {
        T first;
        V second;
    };

    struct OptColor {
        float r;
        float g;
        float b;
        float a;
        bool isSet;

        [[nodiscard]] UnityEngine::Color getColor() const {
            return UnityEngine::Color(r, g, b, a);
        }
    };

    inline OptColor OptColorFromColor(UnityEngine::Color const &color) {
        return {
                color.r,
                color.g,
                color.b,
                color.a,
                true
        };
    }

    // This is probably not needed but it is verbose
    inline const OptColor OptColorNull = {
            0,
            0,
            0,
            0,
            false,
    };


    inline OptColor OptColorFromColor(std::optional<UnityEngine::Color> const &color) {
        return color ? OptColorFromColor(*color) : OptColorNull;
    }
}