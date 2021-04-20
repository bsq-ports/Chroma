#include "main.hpp"
#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "hooks/TrackLaneRingsManager.hpp"
#include "lighting/environment_enhancements/EnvironmentEnhancementManager.hpp"

#include "GlobalNamespace/TrackLaneRing.hpp"

using namespace Chroma;

MAKE_HOOK_OFFSETLESS(TrackLaneRing_FixedUpdateRing, void, GlobalNamespace::TrackLaneRing* self, float fixedDeltaTime) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        TrackLaneRing_FixedUpdateRing(self, fixedDeltaTime);
        return;
    }

    if (!EnvironmentEnhancementManager::SkipRingUpdate.empty())
    {
        auto it = EnvironmentEnhancementManager::SkipRingUpdate.find(self);

        if (it != EnvironmentEnhancementManager::SkipRingUpdate.end()) {
            auto doSkip = it->second;

            if (doSkip) {
                return;
            }
        }
    }

    TrackLaneRing_FixedUpdateRing(self, fixedDeltaTime);
}

MAKE_HOOK_OFFSETLESS(TrackLaneRing_LateUpdateRing, void, GlobalNamespace::TrackLaneRing* self, float interpolationFactor) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        TrackLaneRing_FixedUpdateRing(self, interpolationFactor);
        return;
    }

    if (!EnvironmentEnhancementManager::SkipRingUpdate.empty())
    {
        auto it = EnvironmentEnhancementManager::SkipRingUpdate.find(self);

        if (it != EnvironmentEnhancementManager::SkipRingUpdate.end()) {
            auto doSkip = it->second;

            if (doSkip) {
                return;
            }
        }

    }

    TrackLaneRing_LateUpdateRing(self, interpolationFactor);
}

void Hooks::TrackLaneRing() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), TrackLaneRing_FixedUpdateRing, il2cpp_utils::FindMethodUnsafe("", "TrackLaneRing", "FixedUpdateRing", 1));
    INSTALL_HOOK_OFFSETLESS(getLogger(), TrackLaneRing_LateUpdateRing, il2cpp_utils::FindMethodUnsafe("", "TrackLaneRing", "LateUpdateRing", 1));
    //    INSTALL_HOOK_OFFSETLESS(getLogger(), SaberManager_Finalize, il2cpp_utils::FindMethodUnsafe("System", "Object", "Finalize", 0));
}