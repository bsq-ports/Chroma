#include <string>

#include "Chroma.hpp"
#include "ChromaController.hpp"
#include "GameplayLifetime.hpp"
#include "GameplaySceneLifetime.hpp"

#include "UnityEngine/SceneManagement/SceneManager.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "UnityEngine/SceneManagement/LoadSceneMode.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Chroma;

MAKE_HOOK_MATCH(SceneManager_Internal_ActiveSceneChanged, &UnityEngine::SceneManagement::SceneManager::Internal_ActiveSceneChanged,
                void, UnityEngine::SceneManagement::Scene previousActiveScene, UnityEngine::SceneManagement::Scene newActiveScene) {
  ChromaController::OnActiveSceneChanged(previousActiveScene, newActiveScene);
  SceneManager_Internal_ActiveSceneChanged(previousActiveScene, newActiveScene);
}

static GameplaySceneLifetime gameplayLifetime;

MAKE_HOOK_MATCH(SceneManager_Internal_SceneLoaded, &UnityEngine::SceneManagement::SceneManager::Internal_SceneLoaded,
                void, UnityEngine::SceneManagement::Scene scene, UnityEngine::SceneManagement::LoadSceneMode mode) {
  if (scene.IsValid()) {
    gameplayLifetime.Loaded(scene.get_handle(), std::string(scene.get_name()));
  }
  SceneManager_Internal_SceneLoaded(scene, mode);
}

MAKE_HOOK_MATCH(SceneManager_Internal_SceneUnloaded, &UnityEngine::SceneManagement::SceneManager::Internal_SceneUnloaded,
                void, UnityEngine::SceneManagement::Scene scene) {
  const bool unloadedGameplay = gameplayLifetime.Unloaded(scene.get_handle());
  // Unity callbacks may still use the outgoing gameplay objects. They can also
  // synchronously load a replacement GameCore, whose references must survive.
  SceneManager_Internal_SceneUnloaded(scene);
  if (unloadedGameplay && !gameplayLifetime.HasGameplay()) {
    ClearChromaGameplayReferences();
    ChromaLogger::Logger.info("Released GameCore materials and colorizers");
  }
}

void SceneManager_Internal() {
  INSTALL_HOOK(ChromaLogger::Logger, SceneManager_Internal_ActiveSceneChanged);
  INSTALL_HOOK(ChromaLogger::Logger, SceneManager_Internal_SceneLoaded);
  INSTALL_HOOK(ChromaLogger::Logger, SceneManager_Internal_SceneUnloaded);
}

ChromaInstallHooks(SceneManager_Internal)
