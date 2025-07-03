#include "ChromaHooks.hpp"
#include "ChromaLogger.hpp"

#include "ChromaController.hpp"

#include "GlobalNamespace/GameplayCoreInstaller.hpp"

MAKE_HOOK_MATCH(InstallBindings, &GlobalNamespace::GameplayCoreInstaller::InstallBindings, void,
                GlobalNamespace::GameplayCoreInstaller* self) {
  // Do nothing if Chroma shouldn't run
  Chroma::ChromaController::GameplayCoreInstaller = std::nullopt;
  if (!Chroma::ChromaController::DoChromaHooks()) {
    InstallBindings(self);
    return;
  }


  InstallBindings(self);

  Chroma::ChromaController::GameplayCoreInstaller = self;
}

void InstallGameplayCoreInstallerHooks() {
  INSTALL_HOOK(ChromaLogger::Logger, InstallBindings);
}

ChromaInstallHooks(InstallGameplayCoreInstallerHooks);