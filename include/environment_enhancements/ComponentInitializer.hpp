#pragma once

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include "GameObjectInfo.hpp"
#include "ComponentData.hpp"

namespace UnityEngine {
class Transform;
class GameObject;
} // namespace UnityEngine

namespace Chroma {

class ComponentInitializer {
public:
  static void PrefillComponentsData(UnityEngine::Transform* root,
                                    std::vector<std::shared_ptr<IComponentData>>& componentDatas);
  static void PostfillComponentsData(UnityEngine::Transform* root, UnityEngine::Transform* original,
                                     std::vector<std::shared_ptr<IComponentData>> const& componentDatas);

  static void InitializeCustomComponents(UnityEngine::GameObject* go, rapidjson::Value const& data, bool v2);

  static GameObjectInfo const& InitializeComponents(UnityEngine::Transform* root, UnityEngine::Transform* original,
                                                    std::vector<GameObjectInfo>& gameObjectInfos,
                                                    std::vector<std::shared_ptr<IComponentData>>& componentDatas,
                                                    Zenject::DiContainer* _container);
};

} // namespace Chroma
