#include "main.hpp"
#include "hooks/SaberManager.hpp"
#include "Chroma.hpp"
#include "ChromaController.hpp"

using namespace Chroma;

MAKE_HOOK_OFFSETLESS(SaberManager_Start, void, GlobalNamespace::SaberManager* self) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        SaberManager_Start(self);
        return;
    }

    SaberManagerHolder::saberManager = self;
    SaberManager_Start(self);
}

void Hooks::SaberManager() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), SaberManager_Start, il2cpp_utils::FindMethodUnsafe("", "SaberManager", "Start", 0));
//    INSTALL_HOOK_OFFSETLESS(getLogger(), SaberManager_Finalize, il2cpp_utils::FindMethodUnsafe("System", "Object", "Finalize", 0));
}