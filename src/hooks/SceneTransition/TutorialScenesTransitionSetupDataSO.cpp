#include "Chroma.hpp"

#include "ChromaController.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/TutorialScenesTransitionSetupDataSO.hpp"
#include "colorizer/NoteColorizer.hpp"
#include "UnityEngine/Space.hpp"
#include "UnityEngine/Transform.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Chroma;

MAKE_HOOK_MATCH(TutorialScenesTransitionSetupDataSO_Init, &TutorialScenesTransitionSetupDataSO::Init, void,
                TutorialScenesTransitionSetupDataSO* self,
                GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings) {
  ChromaController::TutorialMode = true;
  TutorialScenesTransitionSetupDataSO_Init(self, playerSpecificSettings);
}

void TutorialScenesTransitionSetupDataSOHook() {
  INSTALL_HOOK(ChromaLogger::Logger, TutorialScenesTransitionSetupDataSO_Init);
}

ChromaInstallHooks(TutorialScenesTransitionSetupDataSOHook)