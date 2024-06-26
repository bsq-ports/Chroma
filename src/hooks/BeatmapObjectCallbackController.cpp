#include "ChromaComponentManager.hpp"

#include "GlobalNamespace/BeatmapCallbacksController.hpp"
#include "GlobalNamespace/BeatmapCallbacksUpdater.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "UnityEngine/Resources.hpp"

using namespace GlobalNamespace;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
custom_types::Helpers::Coroutine updateCoroutines(BeatmapCallbacksController* self) {
  IL2CPP_CATCH_HANDLER(while (true) {
    Chroma::Component::UpdateCoroutines(self);
    co_yield nullptr;
  })
}
#pragma clang diagnostic pop

BeatmapCallbacksController* controller;

MAKE_HOOK_MATCH(BeatmapObjectCallbackController_Start, &BeatmapCallbacksController::ManualUpdate, void,
                BeatmapCallbacksController* self, float songTime) {
  BeatmapObjectCallbackController_Start(self, songTime);
  if (controller != self) {
    controller = self;
    UnityEngine::Resources::FindObjectsOfTypeAll<BeatmapCallbacksUpdater*>().get(0)->StartCoroutine(
        custom_types::Helpers::CoroutineHelper::New(updateCoroutines(self)));
  }
}

void InstallBeatmapObjectCallbackControllerHooks() {
  INSTALL_HOOK(ChromaLogger::Logger, BeatmapObjectCallbackController_Start);
}
ChromaInstallHooks(InstallBeatmapObjectCallbackControllerHooks)