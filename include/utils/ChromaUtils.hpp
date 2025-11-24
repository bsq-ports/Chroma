#pragma once

#include "Chroma.hpp"

#include "sombrero/shared/RandomUtils.hpp"
#include "sombrero/shared/ColorUtils.hpp"
#include "sombrero/shared/Vector3Utils.hpp"
#include "sombrero/shared/QuaternionUtils.hpp"

#include <optional>
#include <string>
#include <random>

#include "tracks/shared/Animation/Track.h"

#include "UnityEngine/Mesh.hpp"
#include "UnityEngine/Transform.hpp"


namespace ChromaUtils {
struct ChromaUtilities {
  inline static std::optional<Sombrero::FastColor> GetColorFromData(rapidjson::Value const& data,
                                                                    const std::string_view member) {
    auto const color = data.FindMember(member.data());

    if (color == data.MemberEnd() || !color->value.IsArray() || color->value.Empty()) return std::nullopt;

    return Sombrero::FastColor(color->value[0].GetFloat(), color->value[1].GetFloat(), color->value[2].GetFloat(),
                               color->value.Size() > 3 ? color->value[3].GetFloat() : 1);
  }

  inline static std::optional<Sombrero::FastColor>
  GetColorFromData(std::optional<std::reference_wrapper<const rapidjson::Value>> const data,
                   const std::string_view member) {
    if (!data) return std::nullopt;

    rapidjson::Value const& unwrapped = *data;

    return GetColorFromData(unwrapped, member);
  }

  template <typename T> inline static auto GetColorFromData(T&& val, bool v2 = false) {
    return GetColorFromData(std::forward<T>(val), v2 ? Chroma::NewConstants::V2_COLOR : Chroma::NewConstants::COLOR);
  }
};

template <typename List, typename Predicate = std::function<bool(typename List::value_type const&)>>
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
  return "{(" + Sombrero::vector3Str(transform->get_position()) + ") (" +
         Sombrero::QuaternionStr(transform->get_rotation()) + ") (" +
         Sombrero::vector3Str(transform->get_localScale()) + ")";
}

// C++ stuff

// Should replace with an actual standard solution
static int constexpr charDiff(char c1, char c2) {
  if (tolower(c1) < tolower(c2)) return -1;
  if (tolower(c1) == tolower(c2)) return 0;
  return 1;
}

static int constexpr stringCompare(std::string const& str1, std::string const& str2) {
  int diff = 0;
  int size = std::min(str1.size(), str2.size());
  for (size_t idx = 0; idx < size && diff == 0; ++idx) {
    diff += charDiff(str1[idx], str2[idx]);
  }
  if (diff != 0) return diff;

  if (str2.length() == str1.length()) return 0;
  if (str2.length() > str1.length()) return 1;
  return -1;
}

template <typename T> static std::optional<T> getIfExists(rapidjson::Value const& val, std::string_view member) {
  if (!val.IsObject() || val.MemberCount() == 0 || val.IsNull()) return std::nullopt;

  auto it = val.FindMember(member.data());
  if (it == val.MemberEnd()) return std::nullopt;

  if (it->value.IsNull()) return std::nullopt;

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
  
  if (it->value.IsNumber()) {
    if constexpr (std::is_same_v<T, bool>) {
      return it->value.GetInt() != 0;
    }
  }

  if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view> ||
                std::is_same_v<T, char const*>) {
    return it->value.GetString();
  } else if constexpr (std::is_same_v<T, rapidjson::Value> || std::is_same_v<T, rapidjson::Value::Object> ||
                       std::is_same_v<T, rapidjson::Value::ConstObject>) {
    return it->value.GetObject();
  } else {
    return it->value.template Get<T>();
  }
}

template <typename T> static T getIfExists(rapidjson::Value const& val, std::string_view member, T const& def) {
  auto opt = getIfExists<T>(val, member);
  if (!opt) return def;

  return *opt;
}

template <typename T>
inline static T getIfExistsOpt(std::optional<std::reference_wrapper<const rapidjson::Value>> const& val,
                               const std::string_view member, T const& def) {
  if (!val) return def;

  return getIfExists<T>(val->get(), member, def);
}

template <typename T>
inline static std::optional<T> getIfExistsOpt(std::optional<std::reference_wrapper<const rapidjson::Value>> const& val,
                                           const std::string_view member) {
  if (!val) return std::nullopt;

  return getIfExists<T>(val->get(), member);
}

template <typename T, typename Value>
constexpr static std::optional<T> getIfExists(Value&& rapidValue, std::span<std::string_view> members) {
  if (!rapidValue) return std::nullopt;

  for (auto member : members) {
    std::optional<T> val = getIfExists(std::forward<Value>(rapidValue), member);

    if (!val) continue;

    return val;
  }

  return std::nullopt;
}

template <typename T, typename Value>
constexpr static std::optional<T> getIfExists(Value&& rapidValue, std::span<std::string_view> members, T const& def) {
  auto val = getIfExists<T, Value>(std::forward<Value>(rapidValue), members);

  return val ? val : def;
}

//    std::optional<float> getIfExistsFloatOpt(std::optional<std::reference_wrapper<rapidjson::Value>> val, const
//    std::string& member) {
//        if (!val ||  val->get().MemberCount() == 0 || !val->get().IsObject()) return std::nullopt;
//
//        auto it = val->get().FindMember(member);
//        if (it == val->get().MemberEnd()) return std::nullopt;
//
//
//        return it->value.GetFloat();
//    }
//
//    float getIfExistsFloat(std::optional<std::reference_wrapper<rapidjson::Value>> val, const std::string& member,
//    float def) {
//        if (!val || !val->get().IsObject() || val->get().Empty()) return def;
//
//        auto it = val->get().FindMember(member);
//        if (it == val->get().MemberEnd()) return def;
//        return it->value.GetFloat();
//    }

template <typename T> inline static constexpr std::optional<T*> ptrToOpt(T* t) {
  return t ? std::make_optional<T*>(t) : std::nullopt;
}

/// <summary>
/// https://answers.unity.com/questions/1594750/is-there-a-premade-triangle-asset.html
/// </summary>
static UnityEngine::Mesh* CreateTriangleMesh() {
  static SafePtrUnity<UnityEngine::Mesh> mesh;
  if(mesh) return mesh.ptr();

  ArrayW<Sombrero::FastVector3> vertices =
      std::initializer_list<Sombrero::FastVector3>({ { -0.5f, -0.5f, 0 }, { 0.5f, -0.5f, 0 }, { 0.0f, 0.5f, 0 } });

  ArrayW<Sombrero::FastVector2> uv = std::initializer_list<Sombrero::FastVector2>({ { 0, 0 }, { 1, 0 }, { 0.5f, 1 } });

  ArrayW<int> triangles = { 0, 1, 2 };

  mesh = UnityEngine::Mesh::New_ctor();
  mesh->set_vertices(reinterpret_cast<Array<UnityEngine::Vector3>*>(vertices.convert()));
  mesh->set_uv(reinterpret_cast<Array<UnityEngine::Vector2>*>(uv.convert()));
  mesh->set_triangles(triangles);

  mesh->RecalculateBounds();
  mesh->RecalculateNormals();
  mesh->RecalculateTangents();
  
  return mesh.ptr();
}

//    static bool ColorEquals(Sombrero::FastColor c1, Sombrero::FastColor& c2) {
//        return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b;
//    }
//
//    static bool ColorEquals(Sombrero::FastColor& c1, Sombrero::FastColor c2) {
//        return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b;
//    }

} // namespace ChromaUtils

// TODO: Replace with il2cpp_utils::AssignableFrom<ParentType*>(ChildOrInstanceType)
#define ASSIGNMENT_CHECK(ParentType, ChildOrInstanceType)                                                              \
  ((ParentType) == (ChildOrInstanceType) || il2cpp_functions::class_is_assignable_from(ParentType, ChildOrInstanceType))