#include "Chroma.hpp"

#include "ChromaController.hpp"

#include "ChromaLogger.hpp"

#include "GlobalNamespace/BeatmapObjectSpawnController.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;
using namespace custom_types::Helpers;
using namespace GlobalNamespace;

MAKE_HOOK_MATCH(BeatmapObjectSpawnController_Start, &BeatmapObjectSpawnController::Start, void,
                BeatmapObjectSpawnController* self) {
  BeatmapObjectSpawnController_Start(self);

  // Do nothing if Chroma shouldn't run
  if (!ChromaController::GetChromaLegacy() && !ChromaController::DoChromaHooks()) {
    return;
  }

  auto delayedStartCoro = CoroutineHelper::New(Chroma::ChromaController::DelayedStartEnumerator(self));

  self->StartCoroutine(delayedStartCoro);
}

void BeatmapObjectSpawnControllerHook() {
  INSTALL_HOOK(ChromaLogger::Logger, BeatmapObjectSpawnController_Start);
}

ChromaInstallHooks(BeatmapObjectSpawnControllerHook)