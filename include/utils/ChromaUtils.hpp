#pragma once


#include "main.hpp"

#include "UnityEngine/Color.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "Chroma.hpp"

#include <optional>
#include <string>

namespace ChromaUtils {
    class ChromaUtilities {
    public:
        static std::optional<UnityEngine::Color> GetColorFromData(rapidjson::Value* data, const std::string& member = Chroma::COLOR);
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
    T getIfExists(rapidjson::Value* val, const std::string& member, T def) {
        if (!val || !val->IsObject() || val->Empty()) return def;

        getLogger().info("Find not optional member %s", member.c_str());
        auto it = val->FindMember(member);

        getLogger().info("Member not optional end", member.c_str());
        if (it == val->MemberEnd()) return def;

        getLogger().info("Get not optional");
        return it->value.Get<T>();
    }

    template<typename T>
    std::optional<T> getIfExists(rapidjson::Value*& val, const std::string& member) {
        if (!val ||  val->MemberCount() == 0 || !val->IsObject()) return std::nullopt;

        getLogger().info("Find member %s", member.c_str());
        auto it = val->FindMember(member);

        getLogger().info("Member end", member.c_str());
        if (it == val->MemberEnd()) return std::nullopt;

        getLogger().info("Get");
        return it->value.Get<T>();
    }


}

#define ASSIGNMENT_CHECK(ParentType, ChildOrInstanceType) il2cpp_functions::class_is_assignable_from(ParentType, ChildOrInstanceType)