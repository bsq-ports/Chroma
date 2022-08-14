#include "Chroma.hpp"

#include "ChromaController.hpp"
#include "colorizer/NoteColorizer.hpp"
#include "environment_enhancements/EnvironmentEnhancementManager.hpp"

#include "GlobalNamespace/BeatmapObjectsAvoidance.hpp"

#include "UnityEngine/Transform.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;
using namespace custom_types::Helpers;
using namespace GlobalNamespace;

MAKE_HOOK_MATCH(BeatmapObjectsAvoidance_Update,&BeatmapObjectsAvoidance::Update, void, BeatmapObjectsAvoidance *self) {
    BeatmapObjectsAvoidance_Update(self);
    if (!ChromaController::DoChromaHooks()) {
        return;
    }

    auto it = EnvironmentEnhancementManager::AvoidancePosition.find(self);

    if (it != EnvironmentEnhancementManager::AvoidancePosition.end()) {
        self->get_transform()->set_localPosition(it->second);
    }

    auto it2 = EnvironmentEnhancementManager::AvoidanceRotation.find(self);

    if (it2 != EnvironmentEnhancementManager::AvoidanceRotation.end()) {
        self->get_transform()->set_localRotation(it2->second);
    }
}

void BeatmapObjectsAvoidanceHook(Logger& logger) {
    INSTALL_HOOK(logger, BeatmapObjectsAvoidance_Update);
}

ChromaInstallHooks(BeatmapObjectsAvoidanceHook)