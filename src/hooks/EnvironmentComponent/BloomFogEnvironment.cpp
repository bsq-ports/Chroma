#include "Chroma.hpp"

#include "ChromaController.hpp"
#include "GlobalNamespace/BloomFogEnvironment.hpp"
#include "GlobalNamespace/BloomFogEnvironmentParams.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;

MAKE_HOOK_MATCH(BloomFogEnvironment_Awake, &BloomFogEnvironment::OnEnable, void, BloomFogEnvironment* self) {
  BloomFogEnvironment_Awake(self);
  if (!ChromaController::DoChromaHooks()) {
    return;
  }

  self->fogParams = Object::Instantiate(self->fogParams);
}

void BloomFogEnvironmentHook(Logger& logger) {
  INSTALL_HOOK(logger, BloomFogEnvironment_Awake);
}

ChromaInstallHooks(BloomFogEnvironmentHook)