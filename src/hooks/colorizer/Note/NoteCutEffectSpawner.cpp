#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/NoteCutCoreEffectsSpawner.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "UnityEngine/Vector3.hpp"
#include "GlobalNamespace/SaberType.hpp"
#include "colorizer/NoteColorizer.hpp"
#include "UnityEngine/Random.hpp"
#include "UnityEngine/Space.hpp"
#include "UnityEngine/Transform.hpp"

#include "hooks/colorizer/Note/ColorManager.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Chroma;

MAKE_HOOK_MATCH(NoteCutEffectSpawner_SpawnNoteCutEffect,
                &NoteCutCoreEffectsSpawner::SpawnNoteCutEffect,
                void,NoteCutCoreEffectsSpawner* self, GlobalNamespace::NoteCutInfo& noteCutInfo, GlobalNamespace::NoteController* noteController) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        NoteCutEffectSpawner_SpawnNoteCutEffect(self, noteCutInfo, noteController);
        return;
    }
    ColorManagerColorForType::EnableColorOverride(noteController);
    NoteCutEffectSpawner_SpawnNoteCutEffect(self, noteCutInfo, noteController);
    ColorManagerColorForType::DisableColorOverride();
}

void NoteCutEffectSpawnerHook(Logger& logger) {
    INSTALL_HOOK(getLogger(), NoteCutEffectSpawner_SpawnNoteCutEffect);
}

ChromaInstallHooks(NoteCutEffectSpawnerHook)