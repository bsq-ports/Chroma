#include "ChromaController.hpp"
#include "main.hpp"
#include "Chroma.hpp"
#include "lighting/ChromaRingsRotationEffect.hpp"
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
    static auto ChromaTrackLaneRingsRotationEffectKlass = classof(Chroma::ChromaRingsRotationEffect*);

    if (self->klass == TrackLaneRingsRotationEffectKlass)
        return;

    if (self->klass == ChromaTrackLaneRingsRotationEffectKlass) {
        reinterpret_cast<Chroma::ChromaRingsRotationEffect*>(self)->AddRingRotationEffectF(angle, step, (float) propagationSpeed, flexySpeed);
        return;
    }

    TrackLaneRingsRotationEffect_AddRingRotationEffect(self, angle, step, propagationSpeed, flexySpeed);
}

MAKE_HOOK_FIND_CLASS_UNSAFE_INSTANCE(ChromaRingsRotationEffect_AddRingRotationEffect, "Chroma", "ChromaRingsRotationEffect", "AddRingRotationEffect", void, GlobalNamespace::TrackLaneRingsRotationEffect* self, float angle, float step, int propagationSpeed, float flexySpeed) {
    reinterpret_cast<Chroma::ChromaRingsRotationEffect *>(self)->AddRingRotationEffectF(angle, step,
                                                                                        (float) propagationSpeed,
                                                                                        flexySpeed);
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
    INSTALL_HOOK_ORIG(getLogger(), TrackLaneRingsRotationEffect_AddRingRotationEffect);
    INSTALL_HOOK_ORIG(getLogger(), ChromaRingsRotationEffect_AddRingRotationEffect);
    INSTALL_HOOK_ORIG(getLogger(), TrackLaneRingsRotationEffect_FixedUpdate);
}

ChromaInstallHooks(TrackLaneRingsRotationEffectHook)