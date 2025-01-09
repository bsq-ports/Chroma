#pragma once

#include "MaterialsManager.hpp"

#include "custom-types/shared/coroutine.hpp"

#include "sombrero/shared/linq_functional.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"
#include "UnityEngine/AsyncOperation.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Shader.hpp"

namespace Chroma {
struct EnvironmentMaterialManager {

  inline static std::unordered_map<ShaderType, SafePtrUnity<UnityEngine::Material>> EnvironmentMaterials;

  static std::optional<UnityEngine::Material*> getMaterial(ShaderType shaderType);

  static custom_types::Helpers::Coroutine Activate();

  static SafePtrUnity<UnityEngine::Shader> waterLit;
};
} // namespace Chroma
