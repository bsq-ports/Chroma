#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "colorizer/Monobehaviours/ChromaSaberController.hpp"

#include "GlobalNamespace/Saber.hpp"
#include "GlobalNamespace/SaberModelContainer.hpp"

#include "UnityEngine/GameObject.hpp"

#include <experimental/coroutine>

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;
using namespace custom_types::Helpers;


MAKE_HOOK_OFFSETLESS(SaberModelContainer_Start, void, SaberModelContainer* self) {
    SaberModelContainer_Start(self);

    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        return;
    }

    self->get_gameObject()->AddComponent<ChromaSaberController*>()->Init(self->saber);
}


void SaberModelContainerHook(Logger& logger) {
    INSTALL_HOOK_OFFSETLESS(logger, SaberModelContainer_Start, il2cpp_utils::FindMethodUnsafe("", "SaberModelContainer", "Start", 0));
}

ChromaInstallHooks(SaberModelContainerHook)