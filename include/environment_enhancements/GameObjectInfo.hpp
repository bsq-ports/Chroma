#pragma once

#include "UnityEngine/GameObject.hpp"

#include <concepts>

namespace Chroma {

class GameObjectInfo {
public:
  std::string FullID;
  size_t FullIDHash; // for faster lookups
  UnityEngine::GameObject* GameObject;

  explicit GameObjectInfo(UnityEngine::GameObject* gameObject);

  [[deprecated("Why copy")]] GameObjectInfo(GameObjectInfo const&) = default;

  [[deprecated("Why copy")]] GameObjectInfo& operator=(GameObjectInfo const&) = default;

  GameObjectInfo(GameObjectInfo&&) = default;
  GameObjectInfo& operator=(GameObjectInfo&&) = default;
};

} // namespace Chroma
