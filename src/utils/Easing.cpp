#include "utils/Easing.hpp"


/// <summary>
/// Constant Pi.
/// </summary>
const float PI = M_PI;

/// <summary>
/// Constant Pi / 2.
/// </summary>
const float HALFPI = M_PI / 2.0f;

using namespace ChromaUtils;

float ChromaUtils::Easings::Interpolate(float p, ChromaUtils::Functions function) {
    switch (function)
    {
        default:
        case Functions::easeLinear: return EaseLinear(p);
        case Functions::easeStep: return EaseStep(p);
        case Functions::easeOutQuad: return EaseOutQuad(p);
        case Functions::easeInQuad: return EaseInQuad(p);
        case Functions::easeInOutQuad: return EaseInOutQuad(p);
        case Functions::easeInCubic: return EaseInCubic(p);
        case Functions::easeOutCubic: return EaseOutCubic(p);
        case Functions::easeInOutCubic: return EaseInOutCubic(p);
        case Functions::easeInQuart: return EaseInQuart(p);
        case Functions::easeOutQuart: return EaseOutQuart(p);
        case Functions::easeInOutQuart: return EaseInOutQuart(p);
        case Functions::easeInQuint: return EaseInQuint(p);
        case Functions::easeOutQuint: return EaseOutQuint(p);
        case Functions::easeInOutQuint: return EaseInOutQuint(p);
        case Functions::easeInSine: return EaseInSine(p);
        case Functions::easeOutSine: return EaseOutSine(p);
        case Functions::easeInOutSine: return EaseInOutSine(p);
        case Functions::easeInCirc: return EaseInCirc(p);
        case Functions::easeOutCirc: return EaseOutCirc(p);
        case Functions::easeInOutCirc: return EaseInOutCirc(p);
        case Functions::easeInExpo: return EaseInExpo(p);
        case Functions::easeOutExpo: return EaseOutExpo(p);
        case Functions::easeInOutExpo: return EaseInOutExpo(p);
        case Functions::easeInElastic: return EaseInElastic(p);
        case Functions::easeOutElastic: return EaseOutElastic(p);
        case Functions::easeInOutElastic: return EaseInOutElastic(p);
        case Functions::easeInBack: return EaseInBack(p);
        case Functions::easeOutBack: return EaseOutBack(p);
        case Functions::easeInOutBack: return EaseInOutBack(p);
        case Functions::easeInBounce: return EaseInBounce(p);
        case Functions::easeOutBounce: return EaseOutBounce(p);
        case Functions::easeInOutBounce: return EaseInOutBounce(p);
    }
}

float ChromaUtils::Easings::EaseLinear(float p) {
    return p;
}

float ChromaUtils::Easings::EaseStep(float p) {
    return std::floor(p);
}

float ChromaUtils::Easings::EaseInQuad(float p) {
    return p * p;
}

float ChromaUtils::Easings::EaseOutQuad(float p) {
    return -(p * (p - 2));
}

float ChromaUtils::Easings::EaseInOutQuad(float p) {
    if (p < 0.5f)
    {
        return 2 * p * p;
    }
    else
    {
        return (-2 * p * p) + (4 * p) - 1;
    }
}

float ChromaUtils::Easings::EaseInCubic(float p) {
    return p * p * p;
}

float ChromaUtils::Easings::EaseOutCubic(float p) {
    float f = p - 1;
    return (f * f * f) + 1;
}

float ChromaUtils::Easings::EaseInOutCubic(float p) {
    if (p < 0.5f)
    {
        return 4 * p * p * p;
    }
    else
    {
        float f = (2 * p) - 2;
        return (0.5f * f * f * f) + 1;
    }
}

float ChromaUtils::Easings::EaseInQuart(float p) {
    return p * p * p * p;
}

float ChromaUtils::Easings::EaseOutQuart(float p) {
    float f = p - 1;
    return (f * f * f * (1 - p)) + 1;
}

float ChromaUtils::Easings::EaseInOutQuart(float p) {
    if (p < 0.5f)
    {
        return 8 * p * p * p * p;
    }
    else
    {
        float f = p - 1;
        return (-8 * f * f * f * f) + 1;
    }
}

float ChromaUtils::Easings::EaseInQuint(float p) {
    return p * p * p * p * p;
}

float ChromaUtils::Easings::EaseOutQuint(float p) {
    float f = p - 1;
    return (f * f * f * f * f) + 1;
}

float ChromaUtils::Easings::EaseInOutQuint(float p) {
    if (p < 0.5f)
    {
        return 16 * p * p * p * p * p;
    }
    else
    {
        float f = (2 * p) - 2;
        return (0.5f * f * f * f * f * f) + 1;
    }
}

float ChromaUtils::Easings::EaseInSine(float p) {
    return std::sin((p - 1) * HALFPI) + 1;
}

float ChromaUtils::Easings::EaseOutSine(float p) {
    return std::sin(p * HALFPI);
}

float ChromaUtils::Easings::EaseInOutSine(float p) {
    return 0.5f * (1 - std::cos(p * PI));
}

float ChromaUtils::Easings::EaseInCirc(float p) {
    return 1 - std::sqrt(1 - (p * p));
}

float ChromaUtils::Easings::EaseOutCirc(float p) {
    return std::sqrt((2 - p) * p);
}

float ChromaUtils::Easings::EaseInOutCirc(float p) {
    if (p < 0.5f)
    {
        return 0.5f * (1 - std::sqrt(1 - (4 * (p * p))));
    }
    else
    {
        return 0.5f * (std::sqrt(-((2 * p) - 3) * ((2 * p) - 1)) + 1);
    }
}

float ChromaUtils::Easings::EaseInExpo(float p) {
    return (p == 0.0f) ? p : std::pow(2.0f, 10.0f * (p - 1.0f));
}

float ChromaUtils::Easings::EaseOutExpo(float p) {
    return (p == 1.0f) ? p : 1.0f - std::pow(2.0f, -10.0f * p);
}

float ChromaUtils::Easings::EaseInOutExpo(float p) {
    if (p == 0.0 || p == 1.0)
    {
        return p;
    }

    if (p < 0.5f)
    {
        return 0.5f * std::pow(2.0f, (20.0f * p) - 10.0f);
    }
    else
    {
        return (-0.5f * std::pow(2.0f, (-20.0f * p) + 10.0f)) + 1.0f;
    }
}

float ChromaUtils::Easings::EaseInElastic(float p) {
    return std::sin(13.0f * HALFPI * p) * std::pow(2.0f, 10.0f * (p - 1.0f));
}

float ChromaUtils::Easings::EaseOutElastic(float p) {
    return (std::sin(-13.0f * HALFPI * (p + 1.0f)) * std::pow(2.0f, -10.0f * p)) + 1.0f;
}

float ChromaUtils::Easings::EaseInOutElastic(float p) {
    if (p < 0.5f)
    {
        return 0.5f * std::sin(13.0f * HALFPI * (2.0f * p)) * std::pow(2.0f, 10.0f * ((2.0f * p) - 1.0f));
    }
    else
    {
        return 0.5f * ((std::sin(-13.0f * HALFPI * (2.0f * p)) * std::pow(2.0f, -10.0f * ((2.0f * p) - 1.0f))) + 2.0f);
    }
}

float ChromaUtils::Easings::EaseInBack(float p) {
    return (p * p * p) - (p * std::sin(p * PI));
}

float ChromaUtils::Easings::EaseOutBack(float p) {
    float f = 1 - p;
    return 1 - ((f * f * f) - (f * std::sin(f * PI)));
}

float ChromaUtils::Easings::EaseInOutBack(float p) {
    if (p < 0.5f)
    {
        float f = 2 * p;
        return 0.5f * ((f * f * f) - (f * std::sin(f * PI)));
    }
    else
    {
        float f = 1 - ((2 * p) - 1);
        return (0.5f * (1 - ((f * f * f) - (f * std::sin(f * PI))))) + 0.5f;
    }
}

float ChromaUtils::Easings::EaseInBounce(float p) {
    return 1 - EaseOutBounce(1 - p);
}

float ChromaUtils::Easings::EaseOutBounce(float p) {
    if (p < 4 / 11.0f)
    {
        return 121 * p * p / 16.0f;
    }
    else if (p < 8 / 11.0f)
    {
        return (363 / 40.0f * p * p) - (99 / 10.0f * p) + (17 / 5.0f);
    }
    else if (p < 9 / 10.0f)
    {
        return (4356 / 361.0f * p * p) - (35442 / 1805.0f * p) + (16061 / 1805.0f);
    }
    else
    {
        return (54 / 5.0f * p * p) - (513 / 25.0f * p) + (268 / 25.0f);
    }
}

float ChromaUtils::Easings::EaseInOutBounce(float p) {
    if (p < 0.5f)
    {
        return 0.5f * EaseInBounce(p * 2);
    }
    else
    {
        return (0.5f * EaseOutBounce((p * 2) - 1)) + 0.5f;
    }
}
