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


}