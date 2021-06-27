#include "Chroma.hpp"
#include "ChromaController.hpp"
#include "colorizer/ObstacleColorizer.hpp"

#include <vector>
#include <optional>

#include "custom-json-data/shared/CustomBeatmapData.h"


#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/register.hpp"
#include "custom-types/shared/coroutine.hpp"

#include "GlobalNamespace/ObstacleDissolve.hpp"

#include "UnityEngine/Color.hpp"
#include "UnityEngine/WaitForEndOfFrame.hpp"

#include <experimental/coroutine>

using namespace CustomJSONData;
using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;
using namespace custom_types::Helpers;

MAKE_HOOK_OFFSETLESS(ObstacleDissolve_Start, void, ObstacleDissolve* self) {
    ObstacleDissolve_Start(self);

    // Do nothing if Chroma shouldn't run
    if (!ChromaController::GetChromaLegacy() && !ChromaController::DoChromaHooks()) {
        return;
    }
    ObstacleColorizer::New(self->obstacleController);
}

MAKE_HOOK_OFFSETLESS(ObstacleDissolve_OnDestroy, void, ObstacleDissolve* self) {
    ObstacleDissolve_OnDestroy(self);

    // Do nothing if Chroma shouldn't run
    if (!ChromaController::GetChromaLegacy() && !ChromaController::DoChromaHooks()) {
        return;
    }

    ObstacleColorizer::Colorizers.erase(self->obstacleController);
}

void Chroma::Hooks::ObstacleDissolve() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), ObstacleDissolve_Start, il2cpp_utils::FindMethodUnsafe("", "ObstacleDissolve", "Start", 0));
    INSTALL_HOOK_OFFSETLESS(getLogger(), ObstacleDissolve_OnDestroy, il2cpp_utils::FindMethodUnsafe("", "ObstacleDissolve", "OnDestroy", 0));
}