#include "ChromaController.hpp"
#include "main.hpp"
#include "Chroma.hpp"
#include "GlobalNamespace/TrackLaneRingsRotationEffect.hpp"

using namespace Chroma;
using namespace GlobalNamespace;

MAKE_HOOK_MATCH(TrackLaneRingsRotationEffect_AddRingRotationEffect, &TrackLaneRingsRotationEffect::AddRingRotationEffect, void, GlobalNamespace::TrackLaneRingsRotationEffect* self, float angle, float step, int propagationSpeed, float flexySpeed) {
    // Essentially, here we cancel the original method. DO NOT call it IF it's a Chroma map
    if (!ChromaController::DoChromaHooks()) {
        TrackLaneRingsRotationEffect_AddRingRotationEffect(self, angle, step, propagationSpeed, flexySpeed);
        return;
    }

    static auto TrackLaneRingsRotationEffectKlass = classof(GlobalNamespace::TrackLaneRingsRotationEffect*);

    if (self->klass == TrackLaneRingsRotationEffectKlass)
        return;

    TrackLaneRingsRotationEffect_AddRingRotationEffect(self, angle, step, propagationSpeed, flexySpeed);
}

MAKE_HOOK_MATCH(TrackLaneRingsRotationEffect_FixedUpdate, &TrackLaneRingsRotationEffect::FixedUpdate, void, GlobalNamespace::TrackLaneRingsRotationEffect* self) {
    // Essentially, here we cancel the original method. DO NOT call it IF it's a Chroma map
    if (!ChromaController::DoChromaHooks()) {
        TrackLaneRingsRotationEffect_FixedUpdate(self);
        return;
    }

    static auto TrackLaneRingsRotationEffectKlass = classof(GlobalNamespace::TrackLaneRingsRotationEffect*);

    if(self->klass == TrackLaneRingsRotationEffectKlass)
        return;

    TrackLaneRingsRotationEffect_FixedUpdate(self);
}

void TrackLaneRingsRotationEffectHook(Logger& logger) {
    // TODO: Remove?
    //    INSTALL_HOOK_OFFSETLESS(getLogger(), TrackLaneRingsRotationEffect_AddRingRotationEffect, il2cpp_utils::FindMethodUnsafe("", "TrackLaneRingsRotationEffect", "AddRingRotationEffect", 4));
    //    INSTALL_HOOK_OFFSETLESS(getLogger(), TrackLaneRingsRotationEffect_FixedUpdate, il2cpp_utils::FindMethodUnsafe("", "TrackLaneRingsRotationEffect", "FixedUpdate", 0));
}

ChromaInstallHooks(TrackLaneRingsRotationEffectHook)