#include "Chroma.hpp"

#include "ChromaController.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;
using namespace custom_types::Helpers;
using namespace GlobalNamespace;

MAKE_HOOK_OFFSETLESS(BeatmapObjectSpawnController_Start, void, BeatmapObjectSpawnController* self) {
    BeatmapObjectSpawnController_Start(self);

    // Do nothing if Chroma shouldn't run
    if (!ChromaController::GetChromaLegacy() && !ChromaController::DoChromaHooks()) {
        return;
    }

    auto delayedStartCoro = CoroutineHelper::New(Chroma::ChromaController::DelayedStartEnumerator(self));

    self->StartCoroutine(reinterpret_cast<enumeratorT*>(delayedStartCoro));
}

void BeatmapObjectSpawnControllerHook(Logger& logger) {
    INSTALL_HOOK_OFFSETLESS(logger, BeatmapObjectSpawnController_Start, il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectSpawnController", "Start", 0));
}

ChromaInstallHooks(BeatmapObjectSpawnControllerHook)