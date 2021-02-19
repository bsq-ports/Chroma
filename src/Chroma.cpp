#include "Chroma.hpp"

void Chroma::InstallHooks() {
    Hooks::LightPairRotationEventEffect();
    Hooks::LightRotationEventEffect();
    Hooks::LightSwitchEventEffect();
}