#pragma once


#include "main.hpp"

#include "UnityEngine/Color.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "Chroma.hpp"

#include <optional>
#include <string>
#include <random>

#define GET_FIND_METHOD(mPtr) il2cpp_utils::il2cpp_type_check::MetadataGetter<mPtr>::get()

namespace ChromaUtils {
    class ChromaUtilities {
    public:
        static std::optional<UnityEngine::Color> GetColorFromData(std::optional<std::reference_wrapper<rapidjson::Value>> data, const std::string& member = Chroma::COLOR);
        static std::optional<UnityEngine::Color> GetColorFromData(rapidjson::Value& data, const std::string& member = Chroma::COLOR);
    };

    inline static std::string vectorStr(UnityEngine::Vector3 vector3) {
        return std::to_string(vector3.x) + ", " + std::to_string(vector3.y) + ", " + std::to_string(vector3.z);
    }

    inline static std::string quaternionStr(UnityEngine::Quaternion vector3) {
        return std::to_string(vector3.x) + ", " + std::to_string(vector3.y) + ", " + std::to_string(vector3.z);
    }

    inline static std::string transformStr(UnityEngine::Transform* transform) {
        return "{(" + vectorStr(transform->get_position()) + ") (" + quaternionStr(transform->get_rotation()) + ") (" + vectorStr(transform->get_localScale()) + ")";
    }

    // C++ stuff

    // Should replace with an actual standard solution
    inline static int charDiff(char c1, char c2)
    {
        if ( tolower(c1) < tolower(c2) ) return -1;
        if ( tolower(c1) == tolower(c2) ) return 0;
        return 1;
    }

    inline static int stringCompare(const std::string& str1, const std::string& str2)
    {
        int diff = 0;
        int size = std::min(str1.size(), str2.size());
        for (size_t idx = 0; idx < size && diff == 0; ++idx)
        {
            diff += charDiff(str1[idx], str2[idx]);
        }
        if ( diff != 0 ) return diff;

        if ( str2.length() == str1.length() ) return 0;
        if ( str2.length() > str1.length() ) return 1;
        return -1;
    }

    template<typename T>
    T getIfExists(std::optional<std::reference_wrapper<rapidjson::Value>> val, const std::string& member, T def) {
        if (!val || !val->get().IsObject() || val->get().Empty()) return def;

        auto it = val->get().FindMember(member);
        if (it == val->get().MemberEnd()) return def;
        return it->value.Get<T>();
    }

    template<typename T>
    std::optional<T> getIfExists(std::optional<std::reference_wrapper<rapidjson::Value>> val, const std::string& member) {
        if (!val || !val->get().IsObject() || val->get().Empty()) return std::nullopt;

        auto it = val->get().FindMember(member);
        if (it == val->get().MemberEnd()) return std::nullopt;


        return it->value.Get<T>();
    }

    // https://stackoverflow.com/a/686373/9816000
    // https://www.cplusplus.com/reference/random/uniform_real_distribution/
    // https://www.delftstack.com/howto/cpp/how-to-generate-random-float-number-in-cpp/
    static float randomNumber(float min, float max = 1.0f) {
        static std::random_device randomDevice;
        static std::default_random_engine eng(randomDevice());
        if (min > max) {
            auto temp = min;
            min = max;
            max = temp;
        }

        std::uniform_real_distribution<float> distr(min, max);

        return distr(eng);
    }

    /// Returns between 0.0f and 1.0f
    static float randomNumber() {
        static std::random_device randomDevice;
        static std::default_random_engine eng(randomDevice());
        static std::uniform_real_distribution<float> distr(0.0f, 1.0f);

        return distr(eng);
    }

    static float Clamp01(float value)
    {
        bool flag = value < 0.0f;
        float result;
        if (flag)
        {
            result = 0.0f;
        }
        else
        {
            bool flag2 = value > 1.0f;
            if (flag2)
            {
                result = 1.0f;
            }
            else
            {
                result = value;
            }
        }
        return result;
    }

    static float Lerp(float a, float b, float t)
    {
        return a + (b - a) * Clamp01(t);
    }

    static UnityEngine::Vector3 vectorAdd(UnityEngine::Vector3 a, UnityEngine::Vector3 b) {
        return UnityEngine::Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
    }

    static UnityEngine::Vector3 vectorMultiply(UnityEngine::Vector3 vector, float m) {
        return UnityEngine::Vector3(vector.x * m, vector.y * m, vector.z * m);
    }

    static UnityEngine::Vector3 quaternionMultiply(UnityEngine::Quaternion rotation, UnityEngine::Vector3 point) {
        float num = rotation.x * 2.0f;
        float num2 = rotation.y * 2.0f;
        float num3 = rotation.z * 2.0f;
        float num4 = rotation.x * num;
        float num5 = rotation.y * num2;
        float num6 = rotation.z * num3;
        float num7 = rotation.x * num2;
        float num8 = rotation.x * num3;
        float num9 = rotation.y * num3;
        float num10 = rotation.w * num;
        float num11 = rotation.w * num2;
        float num12 = rotation.w * num3;
        UnityEngine::Vector3 result;
        result.x = (1.0f - (num5 + num6)) * point.x + (num7 - num12) * point.y + (num8 + num11) * point.z;
        result.y = (num7 + num12) * point.x + (1.0f - (num4 + num6)) * point.y + (num9 - num10) * point.z;
        result.z = (num8 - num11) * point.x + (num9 + num10) * point.y + (1.0f - (num4 + num5)) * point.z;
        return result;
    }

    static UnityEngine::Quaternion quaternionMultiply(UnityEngine::Quaternion lhs, UnityEngine::Quaternion rhs)
    {
        return UnityEngine::Quaternion(lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y, lhs.w * rhs.y + lhs.y * rhs.w + lhs.z * rhs.x - lhs.x * rhs.z, lhs.w * rhs.z + lhs.z * rhs.w + lhs.x * rhs.y - lhs.y * rhs.x, lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z);
    }

    inline static float GammaToLinearSpace(float gamma) {
        return 255.0f * std::pow(gamma / 255.0f, 2.2f);
    }

    inline static float LinearToGammaSpace(float linear) {
        return 255.0f * std::pow(linear / 255.0f, 1.0f/2.2f);
    }

    inline static bool ColorEquals(const UnityEngine::Color c1,const UnityEngine::Color c2) {
        return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b;
    }

    inline static UnityEngine::Color ColorMultiply(const UnityEngine::Color c1,const UnityEngine::Color c2) {
        return UnityEngine::Color(c1.r * c2.r, c1.g * c2.g, c1.b * c2.b, c1.a * c2.a);
    }

    inline static UnityEngine::Color ColorLerp(UnityEngine::Color a, UnityEngine::Color b, float t) {
        t = Clamp01(t);
        return UnityEngine::Color(a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t, a.a + (b.a - a.a) * t);
    }

    inline static UnityEngine::Color ColorLinear(UnityEngine::Color a) {
        return UnityEngine::Color(GammaToLinearSpace(a.r), GammaToLinearSpace(a.g), GammaToLinearSpace(a.b), a.a);
    }

    inline static UnityEngine::Color ColorAlpha(UnityEngine::Color a, float alpha) {
        a.a = alpha;
        return a;
    }

    static void RGBToHSVHelper(float offset, float dominantcolor, float colorone, float colortwo, float& H, float& S, float& V)
    {
        V = dominantcolor;
        bool flag = V != 0.0f;
        if (flag)
        {
            bool flag2 = colorone > colortwo;
            float num;
            if (flag2)
            {
                num = colortwo;
            }
            else
            {
                num = colorone;
            }
            float num2 = V - num;
            bool flag3 = num2 != 0.0f;
            if (flag3)
            {
                S = num2 / V;
                H = offset + (colorone - colortwo) / num2;
            }
            else
            {
                S = 0.0f;
                H = offset + (colorone - colortwo);
            }
            H /= 6.0f;
            bool flag4 = H < 0.0f;
            if (flag4)
            {
                H += 1.0f;
            }
        }
        else
        {
            S = 0.0f;
            H = 0.0f;
        }
    }

    static void ColorRGBToHSV(UnityEngine::Color rgbColor, float& H, float& S, float& V)
    {
        bool flag = rgbColor.b > rgbColor.g && rgbColor.b > rgbColor.r;
        if (flag)
        {
            RGBToHSVHelper(4.0f, rgbColor.b, rgbColor.r, rgbColor.g, H, S, V);
        }
        else
        {
            bool flag2 = rgbColor.g > rgbColor.r;
            if (flag2)
            {
                RGBToHSVHelper(2.0f, rgbColor.g, rgbColor.b, rgbColor.r, H, S, V);
            }
            else
            {
                RGBToHSVHelper(0.0f, rgbColor.r, rgbColor.g, rgbColor.b, H, S, V);
            }
        }
    }

    static UnityEngine::Color ColorHSVToRGB(float H, float S, float V, bool hdr = true)
    {
        auto white = UnityEngine::Color(1.0f, 1.0f, 1.0f, 1.0f);

        bool flag = S == 0.0f;
        if (flag)
        {
            white.r = V;
            white.g = V;
            white.b = V;
        }
        else
        {
            bool flag2 = V == 0.0f;
            if (flag2)
            {
                white.r = 0.0f;
                white.g = 0.0f;
                white.b = 0.0f;
            }
            else
            {
                white.r = 0.0f;
                white.g = 0.0f;
                white.b = 0.0f;
                float num = H * 6.0f;
                int num2 = (int)std::floor(num);
                float num3 = num - (float)num2;
                float num4 = V * (1.0f - S);
                float num5 = V * (1.0f - S * num3);
                float num6 = V * (1.0f - S * (1.0f - num3));
                switch (num2)
                {
                    case -1:
                        white.r = V;
                        white.g = num4;
                        white.b = num5;
                        break;
                    case 0:
                        white.r = V;
                        white.g = num6;
                        white.b = num4;
                        break;
                    case 1:
                        white.r = num5;
                        white.g = V;
                        white.b = num4;
                        break;
                    case 2:
                        white.r = num4;
                        white.g = V;
                        white.b = num6;
                        break;
                    case 3:
                        white.r = num4;
                        white.g = num5;
                        white.b = V;
                        break;
                    case 4:
                        white.r = num6;
                        white.g = num4;
                        white.b = V;
                        break;
                    case 5:
                        white.r = V;
                        white.g = num4;
                        white.b = num5;
                        break;
                    case 6:
                        white.r = V;
                        white.g = num6;
                        white.b = num4;
                        break;
                }
                bool flag3 = !hdr;
                if (flag3)
                {
                    white.r = std::clamp(white.r, 0.0f, 1.0f);
                    white.g = std::clamp(white.g, 0.0f, 1.0f);
                    white.b = std::clamp(white.b, 0.0f, 1.0f);
                }
            }
        }
        return white;
    }

//    static bool ColorEquals(UnityEngine::Color c1, UnityEngine::Color& c2) {
//        return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b;
//    }
//
//    static bool ColorEquals(UnityEngine::Color& c1, UnityEngine::Color c2) {
//        return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b;
//    }


}

// TODO: Replace with il2cpp_utils::AssignableFrom<ParentType*>(ChildOrInstanceType)
#define ASSIGNMENT_CHECK(ParentType, ChildOrInstanceType) il2cpp_functions::class_is_assignable_from(ParentType, ChildOrInstanceType)