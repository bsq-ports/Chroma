#include "Chroma.hpp"

#include "ChromaController.hpp"

#include "GlobalNamespace/BeatmapObjectSpawnController.hpp"

using namespace GlobalNamespace;

MAKE_HOOK_OFFSETLESS(BeatmapObjectSpawnController_Start, void, BeatmapObjectSpawnController* self) {
    BeatmapObjectSpawnController_Start(self);

    self->StartCoroutine(Chroma::ChromaController::DelayedStart(self));
}

void Chroma::Hooks::BeatmapObjectSpawnController() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), BeatmapObjectSpawnController_Start, il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectSpawnController", "Start", 0));
}