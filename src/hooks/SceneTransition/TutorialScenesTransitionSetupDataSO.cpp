#include "Chroma.hpp"

#include "ChromaController.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/TutorialScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "colorizer/NoteColorizer.hpp"
#include "UnityEngine/Space.hpp"
#include "UnityEngine/Transform.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Chroma;

MAKE_HOOK_MATCH(TutorialScenesTransitionSetupDataSO_Init,
                &TutorialScenesTransitionSetupDataSO::Init,
                void,TutorialScenesTransitionSetupDataSO* self) {
    ChromaController::TutorialMode = true;
    TutorialScenesTransitionSetupDataSO_Init(self);
}

void TutorialScenesTransitionSetupDataSOHook(Logger& logger) {
    INSTALL_HOOK(getLogger(), TutorialScenesTransitionSetupDataSO_Init);
}

ChromaInstallHooks(TutorialScenesTransitionSetupDataSOHook)