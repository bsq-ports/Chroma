#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "GlobalNamespace/LightWithIdManager.hpp"


using namespace GlobalNamespace;
using namespace Chroma;

MAKE_HOOK_OFFSETLESS(LightWithIdManager_LateUpdate, void,LightWithIdManager* self) {
    LightWithIdManager_LateUpdate(self);
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        return;
    }

    // For some reason this doesnt get emptied and is continuously looped over.
    // When unregistering a large amount of lights in Chroma, this can add lag, so we dump the list ourselves.
    self->lightsToUnregister->Clear();
}

void Chroma::Hooks::LightWithIdManager() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), LightWithIdManager_LateUpdate, il2cpp_utils::FindMethodUnsafe("", "LightWithIdManager", "LateUpdate", 0));
}