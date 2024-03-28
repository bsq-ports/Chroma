#include "CoreAPI.hpp"
#include "ChromaController.hpp"

using namespace Chroma;
EXPOSE_API(addForceEnableChromaHooks, void, modloader::ModInfo& modInfo) {
  ChromaController::AddForceDoHooks(modInfo);
}

EXPOSE_API(removeForceEnableChromaHooks, void, modloader::ModInfo& modInfo) {
  ChromaController::RemoveForceDoHooks(modInfo);
}
EXPOSE_API(isChromaRunning, bool) {
  return ChromaController::DoChromaHooks();
}
