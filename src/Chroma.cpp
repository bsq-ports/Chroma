#include "Chroma.hpp"

void Chroma::InstallHooks() {
    Hooks::BeatmapObjectSpawnController();
    Hooks::LightPairRotationEventEffect();
    Hooks::LightRotationEventEffect();
    Hooks::LightSwitchEventEffect();
}