#include "ChromaHooks.hpp"
#include "ChromaLogger.hpp"

#include "environment_enhancements/EnvironmentMaterialManager.hpp"

#include "GlobalNamespace/MainSystemInit.hpp"

using namespace GlobalNamespace;
using namespace Chroma;

MAKE_HOOK_MATCH(MainSystemInit_Init, &MainSystemInit::Init, void, MainSystemInit* self,
                ::GlobalNamespace::SettingsApplicatorSO* settingsApplicator) {
  static bool loaded = false;
  if (!loaded) {
    loaded = true;
    self->MonoBehaviour::StartCoroutine(
        custom_types::Helpers::CoroutineHelper::New(EnvironmentMaterialManager::Activate));
  }

  return MainSystemInit_Init(self, settingsApplicator);
}

void MainSystemInitHook() {
  INSTALL_HOOK(ChromaLogger::Logger, MainSystemInit_Init);
}

ChromaInstallHooks(MainSystemInitHook)