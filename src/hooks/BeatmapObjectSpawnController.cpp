#include "Chroma.hpp"

#include "ChromaController.hpp"
#include "utils/CoroutineHelper.hpp"
#include "GlobalNamespace/BeatmapObjectSpawnController.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;

using namespace GlobalNamespace;

MAKE_HOOK_OFFSETLESS(BeatmapObjectSpawnController_Start, void, BeatmapObjectSpawnController* self) {
    auto delayedStartCoro = Chroma::ChromaController::DelayedStartEnumerator(self);

    self->StartCoroutine(reinterpret_cast<IEnumerator*>(&delayedStartCoro));

    BeatmapObjectSpawnController_Start(self);
}

void Chroma::Hooks::BeatmapObjectSpawnController() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), BeatmapObjectSpawnController_Start, il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectSpawnController", "Start", 0));
}