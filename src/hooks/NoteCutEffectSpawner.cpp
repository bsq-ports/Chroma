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

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Chroma;

MAKE_HOOK_OFFSETLESS(NoteCutEffectSpawner_SpawnNoteCutEffect,void,NoteCutCoreEffectsSpawner* self, UnityEngine::Vector3 pos, GlobalNamespace::NoteController* noteController, GlobalNamespace::NoteCutInfo* noteCutInfo) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        NoteCutEffectSpawner_SpawnNoteCutEffect(self, pos, noteController, noteCutInfo);
        return;
    }
    NoteColorizer::EnableNoteColorOverride(noteController);
    NoteCutEffectSpawner_SpawnNoteCutEffect(self, pos, noteController, noteCutInfo);
    NoteColorizer::DisableNoteColorOverride();
}

void Chroma::Hooks::NoteCutEffectSpawner() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), NoteCutEffectSpawner_SpawnNoteCutEffect, il2cpp_utils::FindMethodUnsafe("", "NoteCutCoreEffectsSpawner", "SpawnNoteCutEffect", 3));
}