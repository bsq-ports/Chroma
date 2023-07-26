#include "Chroma.hpp"
#include "ChromaHooks.hpp"

#include "environment_enhancements/EnvironmentMaterialManager.hpp"

#include "GlobalNamespace/MainSystemInit.hpp"

using namespace GlobalNamespace;
using namespace Chroma;

MAKE_HOOK_MATCH(MainSystemInit_Init, &MainSystemInit::Init, void, MainSystemInit* self) {
  static bool loaded = true;
  if (!loaded) {
    loaded = true;
    self->MonoBehaviour::StartCoroutine(
        custom_types::Helpers::CoroutineHelper::New(EnvironmentMaterialManager::Activate));
  }

  return MainSystemInit_Init(self);
}

void MainSystemInitHook(Logger& /*logger*/) {
  INSTALL_HOOK(getLogger(), MainSystemInit_Init);
}

ChromaInstallHooks(MainSystemInitHook)