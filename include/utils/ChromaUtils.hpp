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

    inline static bool ColorEquals(const UnityEngine::Color c1,const UnityEngine::Color c2) {
        return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b;
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