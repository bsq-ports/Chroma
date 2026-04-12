#include "Chroma.hpp"
#include "ChromaController.hpp"

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

void SceneManager_Internal() {
  INSTALL_HOOK(ChromaLogger::Logger, SceneManager_Internal_ActiveSceneChanged);
}

ChromaInstallHooks(SceneManager_Internal)