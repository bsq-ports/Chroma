#pragma once

#include "environment_enhancements/MaterialsManager.hpp"

#include "UnityEngine/MonoBehaviour.hpp"

#include "custom-types/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(Chroma, MaterialAnimator, UnityEngine::MonoBehaviour) {
public:
  std::vector<MaterialInfo> materials;
  TimeUnit lastCheckedTime;

  DECLARE_INSTANCE_METHOD(void, Update);
};