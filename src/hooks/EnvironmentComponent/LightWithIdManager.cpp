#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "GlobalNamespace/LightWithIdManager.hpp"


using namespace GlobalNamespace;
using namespace Chroma;

MAKE_HOOK_MATCH(LightWithIdManager_LateUpdate, &LightWithIdManager::LateUpdate, void,LightWithIdManager* self) {
    LightWithIdManager_LateUpdate(self);
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        return;
    }

    // For some reason this doesnt get emptied and is continuously looped over.
    // When unregistering a large amount of lights in Chroma, this can add lag, so we dump the list ourselves.
    self->lightsToUnregister->Clear();
}

void LightWithIdManagerHook(Logger& logger) {
    INSTALL_HOOK(logger, LightWithIdManager_LateUpdate);
}

ChromaInstallHooks(LightWithIdManagerHook)