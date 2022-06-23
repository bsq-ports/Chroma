#include "main.hpp"
#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "environment_enhancements/ComponentInitializer.hpp"
#include "hooks/TrackLaneRingsManager.hpp"

#include "GlobalNamespace/TrackLaneRingsManager.hpp"

using namespace Chroma;
using namespace GlobalNamespace;

std::vector<GlobalNamespace::TrackLaneRingsManager*> TrackLaneRingsManagerHolder::RingManagers = std::vector<GlobalNamespace::TrackLaneRingsManager*>();

MAKE_HOOK_MATCH(TrackLaneRingsManager_Awake, &TrackLaneRingsManager::Awake, void, GlobalNamespace::TrackLaneRingsManager* self) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        TrackLaneRingsManager_Awake(self);
        return;
    }

    if (ComponentInitializer::SkipAwake) {
        return;
    }

    TrackLaneRingsManager_Awake(self);
    TrackLaneRingsManagerHolder::RingManagers.push_back(self);
}

void TrackLaneRingsManagerHook(Logger& logger) {
    INSTALL_HOOK(logger, TrackLaneRingsManager_Awake);
//    INSTALL_HOOK_OFFSETLESS(getLogger(), SaberManager_Finalize, il2cpp_utils::FindMethodUnsafe("System", "Object", "Finalize", 0));
}

ChromaInstallHooks(TrackLaneRingsManagerHook)