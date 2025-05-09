#include "Chroma.hpp"
#include "ChromaController.hpp"
#include "ChromaLogger.hpp"

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

  self->_fogParams = Object::Instantiate(self->fogParams);
}

void BloomFogEnvironmentHook() {
  INSTALL_HOOK(ChromaLogger::Logger, BloomFogEnvironment_Awake);
}

ChromaInstallHooks(BloomFogEnvironmentHook)