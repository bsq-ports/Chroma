#include "Chroma.hpp"

#include "colorizer/LightColorizer.hpp"
#include "GlobalNamespace/LightSwitchEventEffect.hpp"

using namespace Chroma;
using namespace GlobalNamespace;

MAKE_HOOK_OFFSETLESS(LightSwitchEventEffect_Start, void, LightSwitchEventEffect* self) {
    LightColorizer::LSEStart(self, &self->event);

    LightSwitchEventEffect_Start(self);
}

void Chroma::Hooks::LightSwitchEventEffect() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), LightSwitchEventEffect_Start, il2cpp_utils::FindMethodUnsafe("", "LightSwitchEventEffect", "Start", 0));
}