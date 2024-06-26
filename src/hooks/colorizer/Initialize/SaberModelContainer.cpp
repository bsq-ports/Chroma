#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "colorizer/Monobehaviours/ChromaSaberController.hpp"

#include "GlobalNamespace/Saber.hpp"
#include "GlobalNamespace/SaberModelContainer.hpp"

#include "UnityEngine/GameObject.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;
using namespace custom_types::Helpers;

MAKE_HOOK_MATCH(SaberModelContainer_Start, &SaberModelContainer::Start, void, SaberModelContainer* self) {
  SaberModelContainer_Start(self);

  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    return;
  }

  self->get_gameObject()->AddComponent<ChromaSaberController*>()->Init(self->_saber);
}

void SaberModelContainerHook() {
  INSTALL_HOOK(ChromaLogger::Logger, SaberModelContainer_Start);
}

ChromaInstallHooks(SaberModelContainerHook)