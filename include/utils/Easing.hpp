#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include <map>
#include <string>

#define Functions_ENUM(DO) \
        DO(easeLinear) \
        DO(easeStep) \
        DO(easeInQuad) \
        DO(easeOutQuad) \
        DO(easeInOutQuad) \
        DO(easeInCubic) \
        DO(easeOutCubic) \
        DO(easeInOutCubic) \
        DO(easeInQuart) \
        DO(easeOutQuart) \
        DO(easeInOutQuart) \
        DO(easeInQuint) \
        DO(easeOutQuint) \
        DO(easeInOutQuint) \
        DO(easeInSine) \
        DO(easeOutSine) \
        DO(easeInOutSine) \
        DO(easeInCirc) \
        DO(easeOutCirc) \
        DO(easeInOutCirc) \
        DO(easeInExpo) \
        DO(easeOutExpo) \
        DO(easeInOutExpo) \
        DO(easeInElastic) \
        DO(easeOutElastic) \
        DO(easeInOutElastic) \
        DO(easeInBack) \
        DO(easeOutBack) \
        DO(easeInOutBack) \
        DO(easeInBounce) \
        DO(easeOutBounce) \
        DO(easeInOutBounce)

namespace ChromaUtils {

    enum class Functions {
        #define MAKE_ENUM(VAR) VAR,
        Functions_ENUM(MAKE_ENUM)
        #undef MAKE_ENUM
    };

    inline static const std::map<std::string, int> FUNCTION_NAMES {
        #define MAKE_NAMES(VAR) {#VAR, (int)ChromaUtils::Functions::VAR},
                Functions_ENUM(MAKE_NAMES)
        #undef MAKE_NAMES
    };


    class Easings {
    public:
        /// <summary>
        /// Interpolate using the specified function.
        /// </summary>
        static float Interpolate(float p, Functions function);

        /// <summary>
        /// Modeled after the line y = x
        /// </summary>
        static float EaseLinear(float p);

        /// <summary>
        /// It's either 1, or it's not
        /// </summary>
        static float EaseStep(float p);

        /// <summary>
        /// Modeled after the parabola y = x^2
        /// </summary>
        static float EaseInQuad(float p);

        /// <summary>
        /// Modeled after the parabola y = -x^2 + 2x
        /// </summary>
        static float EaseOutQuad(float p);

        /// <summary>
        /// Modeled after the piecewise quad
        /// y = (1/2)((2x)^2)             ; [0, 0.5)
        /// y = -(1/2)((2x-1)*(2x-3) - 1) ; [0.5, 1]
        /// </summary>
        static float EaseInOutQuad(float p);

        /// <summary>
        /// Modeled after the cubic y = x^3
        /// </summary>
        static float EaseInCubic(float p);

        /// <summary>
        /// Modeled after the cubic y = (x - 1)^3 + 1
        /// </summary>
        static float EaseOutCubic(float p);

        /// <summary>
        /// Modeled after the piecewise cubic
        /// y = (1/2)((2x)^3)       ; [0, 0.5)
        /// y = (1/2)((2x-2)^3 + 2) ; [0.5, 1]
        /// </summary>
        static float EaseInOutCubic(float p);

        /// <summary>
        /// Modeled after the quart x^4
        /// </summary>
        static float EaseInQuart(float p);

        /// <summary>
        /// Modeled after the quart y = 1 - (x - 1)^4
        /// </summary>
        static float EaseOutQuart(float p);

        /// <summary>
        /// Modeled after the piecewise quart
        /// y = (1/2)((2x)^4)        ; [0, 0.5)
        /// y = -(1/2)((2x-2)^4 - 2) ; [0.5, 1]
        /// </summary>
        static float EaseInOutQuart(float p);

        /// <summary>
        /// Modeled after the quint y = x^5
        /// </summary>
        static float EaseInQuint(float p);

        /// <summary>
        /// Modeled after the quint y = (x - 1)^5 + 1
        /// </summary>
        static float EaseOutQuint(float p);

        /// <summary>
        /// Modeled after the piecewise quint
        /// y = (1/2)((2x)^5)       ; [0, 0.5)
        /// y = (1/2)((2x-2)^5 + 2) ; [0.5, 1]
        /// </summary>
        static float EaseInOutQuint(float p);

        /// <summary>
        /// Modeled after quarter-cycle of sine wave
        /// </summary>
        static float EaseInSine(float p);

        /// <summary>
        /// Modeled after quarter-cycle of sine wave (different phase)
        /// </summary>
        static float EaseOutSine(float p);

        /// <summary>
        /// Modeled after half sine wave
        /// </summary>
        static float EaseInOutSine(float p);

        /// <summary>
        /// Modeled after shifted quadrant IV of unit circle
        /// </summary>
        static float EaseInCirc(float p);

        /// <summary>
        /// Modeled after shifted quadrant II of unit circle
        /// </summary>
        static float EaseOutCirc(float p);

        /// <summary>
        /// Modeled after the piecewise circ function
        /// y = (1/2)(1 - Math.Sqrt(1 - 4x^2))           ; [0, 0.5)
        /// y = (1/2)(Math.Sqrt(-(2x - 3)*(2x - 1)) + 1) ; [0.5, 1]
        /// </summary>
        static float EaseInOutCirc(float p);

        /// <summary>
        /// Modeled after the expo function y = 2^(10(x - 1))
        /// </summary>
        static float EaseInExpo(float p);

        /// <summary>
        /// Modeled after the expo function y = -2^(-10x) + 1
        /// </summary>
        static float EaseOutExpo(float p);

        /// <summary>
        /// Modeled after the piecewise expo
        /// y = (1/2)2^(10(2x - 1))         ; [0,0.5)
        /// y = -(1/2)*2^(-10(2x - 1))) + 1 ; [0.5,1]
        /// </summary>
        static float EaseInOutExpo(float p);

        /// <summary>
        /// Modeled after the damped sine wave y = sin(13pi/2*x)*Math.Pow(2, 10 * (x - 1))
        /// </summary>
        static float EaseInElastic(float p);

        /// <summary>
        /// Modeled after the damped sine wave y = sin(-13pi/2*(x + 1))*Math.Pow(2, -10x) + 1
        /// </summary>
        static float EaseOutElastic(float p);

        /// <summary>
        /// Modeled after the piecewise expoly-damped sine wave:
        /// y = (1/2)*sin(13pi/2*(2*x))*Math.Pow(2, 10 * ((2*x) - 1))      ; [0,0.5)
        /// y = (1/2)*(sin(-13pi/2*((2x-1)+1))*Math.Pow(2,-10(2*x-1)) + 2) ; [0.5, 1]
        /// </summary>
        static float EaseInOutElastic(float p);

        /// <summary>
        /// Modeled after the overshooting cubic y = x^3-x*sin(x*pi)
        /// </summary>
        static float EaseInBack(float p);

        /// <summary>
        /// Modeled after overshooting cubic y = 1-((1-x)^3-(1-x)*sin((1-x)*pi))
        /// </summary>
        static float EaseOutBack(float p);

        /// <summary>
        /// Modeled after the piecewise overshooting cubic function:
        /// y = (1/2)*((2x)^3-(2x)*sin(2*x*pi))           ; [0, 0.5)
        /// y = (1/2)*(1-((1-x)^3-(1-x)*sin((1-x)*pi))+1) ; [0.5, 1]
        /// </summary>
        static float EaseInOutBack(float p);

        static float EaseInBounce(float p);

        static float EaseOutBounce(float p);

        static float EaseInOutBounce(float p);
    };
}