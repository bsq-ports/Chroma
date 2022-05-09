#include "Chroma.hpp"

#include "ChromaController.hpp"
#include "colorizer/SliderColorizer.hpp"

#include "GlobalNamespace/SliderController.hpp"


using namespace GlobalNamespace;
using namespace Chroma;


MAKE_HOOK_MATCH(SliderController_Awake,
                &SliderController::Awake,
                void, SliderController* self) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        SliderController_Awake(self);
        return;
    }

    SliderController_Awake(self);
    SliderColorizer::New(self);
}

MAKE_HOOK_MATCH(SliderController_OnDestroy,
                &SliderController::OnDestroy,
                void, SliderController* self) {
    SliderController_OnDestroy(self);
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        return;
    }

    SliderColorizer::Colorizers.erase(self);
}

void BaseNoteVisualsHook(Logger& logger) {
    INSTALL_HOOK(logger, SliderController_Awake);
    INSTALL_HOOK(logger, SliderController_OnDestroy);
}

ChromaInstallHooks(BaseNoteVisualsHook)