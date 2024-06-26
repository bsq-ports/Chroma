#include "environment_enhancements/EnvironmentMaterialManager.hpp"

custom_types::Helpers::Coroutine Chroma::EnvironmentMaterialManager::Activate() {
  using namespace Sombrero::Linq::Functional;

  CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Waiting to scene load");

  for (int i = 0; i < 90 * 2; i++) {
    co_yield nullptr;
  }

  CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Beginning scene load");

  auto Load = [](std::string_view environmentName) {
    CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Loading environment [{}].", environmentName);
    return UnityEngine::SceneManagement::SceneManager::LoadSceneAsync(
        environmentName, UnityEngine::SceneManagement::LoadSceneMode::Additive);
  };

  auto environments =
      std::array<std::string_view, 3>({ "BTSEnvironment", "BillieEnvironment", "InterscopeEnvironment" });
  auto loads = environments | Select([&](std::string_view s) { return Load(s); });

  for (auto const& n : loads) {
    if (!n) continue;

    while (!n->get_isDone()) {
      co_yield nullptr;
    }
  }

  auto environmentMaterials = UnityEngine::Resources::FindObjectsOfTypeAll<UnityEngine::Material*>();

  auto Save = [&](ShaderType key, std::string_view matName) {
    auto* material = environmentMaterials->FirstOrDefault([&](auto const& e) { return e->get_name() == matName; });
    if (material != nullptr) {
      EnvironmentMaterials[key] = material;
      CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Saving [{}] to [{}].", matName, static_cast<int>(key));
    } else {
      CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Could not find [{}].", matName);
    }
  };

  Save(ShaderType::BTSPillar, "BTSDarkEnvironmentWithHeightFog");
  Save(ShaderType::BillieWater, "WaterfallFalling");
  Save(ShaderType::WaterfallMirror, "WaterfallMirror");
  Save(ShaderType::InterscopeConcrete, "Concrete2");
  Save(ShaderType::InterscopeCar, "Car");

  for (auto const& environment : environments) {
    //UnityEngine::SceneManagement::SceneManager::UnloadSceneAsync(environment);
  }

  co_return;
}

std::optional<UnityEngine::Material*> Chroma::EnvironmentMaterialManager::getMaterial(Chroma::ShaderType shaderType) {
  auto it = EnvironmentMaterials.find(shaderType);

  if (it == EnvironmentMaterials.end()) {
    return std::nullopt;
  }

  return it->second.ptr();
}
