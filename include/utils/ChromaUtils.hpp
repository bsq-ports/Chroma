#pragma once


#include "main.hpp"

#include "sombrero/shared/RandomUtils.hpp"
#include "sombrero/shared/ColorUtils.hpp"
#include "sombrero/shared/Vector3Utils.hpp"
#include "sombrero/shared/QuaternionUtils.hpp"

#include "UnityEngine/Color.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "Chroma.hpp"

#include <optional>
#include <string>
#include <random>
#include "tracks/shared/Animation/Track.h"

#define GET_FIND_METHOD(mPtr) il2cpp_utils::il2cpp_type_check::MetadataGetter<mPtr>::get()

namespace ChromaUtils {
class ChromaUtilities {
public:
    static std::optional<Sombrero::FastColor>
    GetColorFromData(rapidjson::Value const &data, const std::string_view member = Chroma::COLOR) {
        auto const color = data.FindMember(member.data());

        if (color == data.MemberEnd() || !color->value.IsArray() || color->value.Empty())
            return std::nullopt;


        return Sombrero::FastColor(color->value[0].GetFloat(), color->value[1].GetFloat(), color->value[2].GetFloat(),
                                   color->value.Size() > 3 ? color->value[3].GetFloat() : 1);
    }

    inline static std::optional<Sombrero::FastColor> GetColorFromData(std::optional<std::reference_wrapper<const rapidjson::Value>> const data,
                                                                      const std::string_view member = Chroma::COLOR) {
        if (!data) return std::nullopt;

        rapidjson::Value const &unwrapped = *data;

        return GetColorFromData(unwrapped, member);
    }
};


    template<typename List, typename Predicate = std::function<bool(typename List::value_type const&)> >
    int FindIndex(List const& list, Predicate const& predicate, int startIndex = 0, int endIndex = -1) {
        if (endIndex == -1) endIndex = list.size();

        for (int i = startIndex; i < endIndex; i++) {
            auto const& element = list[i];
            if (predicate(element)) {
                return i;
            }
        }

        return -1;
    }

    inline static std::string transformStr(UnityEngine::Transform* transform) {
        return "{(" + Sombrero::vector3Str(transform->get_position()) + ") (" + Sombrero::QuaternionStr(transform->get_rotation()) + ") (" + Sombrero::vector3Str(transform->get_localScale()) + ")";
    }

    // C++ stuff

    // Should replace with an actual standard solution
    static int constexpr charDiff(char c1, char c2)
    {
        if ( tolower(c1) < tolower(c2) ) return -1;
        if ( tolower(c1) == tolower(c2) ) return 0;
        return 1;
    }

    static int constexpr stringCompare(const std::string& str1, const std::string& str2)
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
    static T getIfExists(rapidjson::Value const& val, std::string_view member, T const& def) {
        if (!val.IsObject() || val.Empty() || val.IsNull()) return def;

        auto it = val.FindMember(member.data());
        if (it == val.MemberEnd()) return def;

        if (it->value.IsNull())
            return def;

        if (it->value.IsString()) {
            if constexpr (std::is_same_v<T, bool>) {
                return std::string(it->value.GetString()) == "true";
            }

            if constexpr (std::is_same_v<T, float>) {
                return std::stof(it->value.GetString());
            }

            if constexpr (std::is_same_v<T, int> || std::is_enum_v<T>) {
                return std::stoi(it->value.GetString());
            }
        }

        return it->value.Get<T>();
    }

    template<typename T>
    static std::optional<T> getIfExists(rapidjson::Value const& val, std::string_view member) {
        if (!val.IsObject() || val.Empty() || val.IsNull()) return std::nullopt;

        auto it = val.FindMember(member.data());
        if (it == val.MemberEnd()) return std::nullopt;

        if (it->value.IsNull())
            return std::nullopt;

        if (it->value.IsString()) {
            if constexpr (std::is_same_v<T, bool>) {
                return std::string(it->value.GetString()) == "true";
            }

            if constexpr (std::is_same_v<T, float>) {
                return std::stof(it->value.GetString());
            }

            if constexpr (std::is_same_v<T, int>) {
                return std::stoi(it->value.GetString());
            }

            if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view> || std::is_same_v<T, char const*>) {
                return it->value.GetString();
            }
        }

        return it->value.Get<T>();
    }

    template<typename T>
    inline static T getIfExists(std::optional<std::reference_wrapper<const rapidjson::Value>> const& val, const std::string_view member, T const& def) {
        if (!val) return def;

        return getIfExists<T>(val->get(), member, def);
    }

    template<typename T>
    inline static std::optional<T> getIfExists(std::optional<std::reference_wrapper<const rapidjson::Value>> const& val, const std::string_view member) {
        if (!val) return std::nullopt;

        return getIfExists<T>(val->get(), member);
    }

//    std::optional<float> getIfExistsFloatOpt(std::optional<std::reference_wrapper<rapidjson::Value>> val, const std::string& member) {
//        if (!val ||  val->get().MemberCount() == 0 || !val->get().IsObject()) return std::nullopt;
//
//        auto it = val->get().FindMember(member);
//        if (it == val->get().MemberEnd()) return std::nullopt;
//
//
//        return it->value.GetFloat();
//    }
//
//    float getIfExistsFloat(std::optional<std::reference_wrapper<rapidjson::Value>> val, const std::string& member, float def) {
//        if (!val || !val->get().IsObject() || val->get().Empty()) return def;
//
//        auto it = val->get().FindMember(member);
//        if (it == val->get().MemberEnd()) return def;
//        return it->value.GetFloat();
//    }

    template<typename T>
    inline static constexpr std::optional<T*> ptrToOpt(T* t) {
        return t ? std::make_optional<T*>(t) : std::nullopt;
    }

//    static bool ColorEquals(Sombrero::FastColor c1, Sombrero::FastColor& c2) {
//        return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b;
//    }
//
//    static bool ColorEquals(Sombrero::FastColor& c1, Sombrero::FastColor c2) {
//        return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b;
//    }


}

// TODO: Replace with il2cpp_utils::AssignableFrom<ParentType*>(ChildOrInstanceType)
#define ASSIGNMENT_CHECK(ParentType, ChildOrInstanceType) ((ParentType) == (ChildOrInstanceType) || il2cpp_functions::class_is_assignable_from(ParentType, ChildOrInstanceType))