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



    // Idk if this is better, I just know it works
    inline uint8_t floatToByte(float c) {
        return (uint8_t) std::floor(double(c * 255));
    }

    inline float byteToFloat(uint8_t c) {
        return (float) c / 255.0f;
    }

    template<typename T, typename V>
    struct ExternPair {
        T first;
        V second;
    };

    struct OptColor
    {
        std::uint8_t r;
        std::uint8_t g;
        std::uint8_t b : 7;
        bool isSet : 1;
        std::uint8_t a;

        [[nodiscard]] UnityEngine::Color getColor() const {
            return UnityEngine::Color(byteToFloat(r), byteToFloat(g), byteToFloat(b), byteToFloat(a));
        }
    };

    inline OptColor OptColorFromColor(UnityEngine::Color const& color) {
        return {
                floatToByte(color.r),
                floatToByte(color.g),
                floatToByte(color.b),
                true,
                floatToByte(color.a),
        };
    }

    // This is probably not needed but it is verbose
    inline OptColor OptColorNull() {
        return {
                0,
                0,
                0,
                false,
                0,
        };
    }


    static_assert(sizeof(OptColor) == 4);
}