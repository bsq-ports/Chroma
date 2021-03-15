#include "ChromaController.hpp"
#include "main.hpp"
#include "Chroma.hpp"
#include "GlobalNamespace/TrackLaneRingsRotationEffect.hpp"

using namespace Chroma;

MAKE_HOOK_OFFSETLESS(TrackLaneRingsRotationEffect_Start, void, GlobalNamespace::TrackLaneRingsRotationEffect* self) {
    // Essentially, here we cancel the original method. DO NOT call it IF it's a Chroma map
    if (!ChromaController::DoChromaHooks()) {
        TrackLaneRingsRotationEffect_Start(self);
        return;
    }

}

void Hooks::TrackLaneRingsRotationEffect() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), TrackLaneRingsRotationEffect_Start, il2cpp_utils::FindMethodUnsafe("", "TrackLaneRingsRotationEffect", "Start", 0));
//    INSTALL_HOOK_OFFSETLESS(getLogger(), SaberManager_Finalize, il2cpp_utils::FindMethodUnsafe("System", "Object", "Finalize", 0));
}