#include "CoreAPI.hpp"
#include "ChromaController.hpp"

using namespace Chroma;
EXPOSE_API(addForceEnableChromaHooks, void, ModInfo& modInfo) {
  ChromaController::AddForceDoHooks(modInfo);
}

EXPOSE_API(removeForceEnableChromaHooks, void, ModInfo& modInfo) {
  ChromaController::RemoveForceDoHooks(modInfo);
}
EXPOSE_API(isChromaRunning, bool) {
  return ChromaController::DoChromaHooks();
}
