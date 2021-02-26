#include "main.hpp"
#include "Chroma.hpp"
#include "GlobalNamespace/TrackLaneRingsRotationEffect.hpp"

using namespace Chroma;

MAKE_HOOK_OFFSETLESS(TrackLaneRingsRotationEffect_Start, void, GlobalNamespace::TrackLaneRingsRotationEffect* self) {
    // TODO: Make this ONLY run with Chroma maps
    // Essentially, here we cancel it. DO NOT call it

}

void Hooks::TrackLaneRingsRotationEffect() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), TrackLaneRingsRotationEffect_Start, il2cpp_utils::FindMethodUnsafe("", "TrackLaneRingsRotationEffect", "Start", 0));
//    INSTALL_HOOK_OFFSETLESS(getLogger(), SaberManager_Finalize, il2cpp_utils::FindMethodUnsafe("System", "Object", "Finalize", 0));
}