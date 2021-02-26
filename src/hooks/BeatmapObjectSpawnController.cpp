#include "Chroma.hpp"

#include "ChromaController.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;
using namespace custom_types::Helpers;
using namespace GlobalNamespace;

MAKE_HOOK_OFFSETLESS(BeatmapObjectSpawnController_Start, void, BeatmapObjectSpawnController* self) {
    auto delayedStartCoro = CoroutineHelper::New(Chroma::ChromaController::DelayedStartEnumerator(self));

    self->StartCoroutine(reinterpret_cast<enumeratorT*>(delayedStartCoro));

    BeatmapObjectSpawnController_Start(self);
}

void Chroma::Hooks::BeatmapObjectSpawnController() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), BeatmapObjectSpawnController_Start, il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectSpawnController", "Start", 0));
}