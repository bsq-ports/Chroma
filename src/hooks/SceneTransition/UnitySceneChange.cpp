#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "GlobalNamespace/GameScenesManager.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "UnityEngine/SceneManagement/LoadSceneMode.hpp"
#include "System/Action.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"


using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Chroma;

MAKE_HOOK_OFFSETLESS(SceneManager_Internal_SceneLoaded, void, UnityEngine::SceneManagement::Scene scene, UnityEngine::SceneManagement::LoadSceneMode mode) {
    ChromaController::OnActiveSceneChanged(scene);
    SceneManager_Internal_SceneLoaded(scene, mode);
}

void SceneManager_Internal(Logger& logger) {
    INSTALL_HOOK_OFFSETLESS(getLogger(), SceneManager_Internal_SceneLoaded, il2cpp_utils::FindMethodUnsafe("UnityEngine.SceneManagement", "SceneManager", "Internal_SceneLoaded", 2));
}

ChromaInstallHooks(SceneManager_Internal)