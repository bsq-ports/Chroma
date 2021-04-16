#include "Chroma.hpp"

void Chroma::InstallHooks() {
    Hooks::BeatEffectSpawner();
    Hooks::BeatmapDataTransformHelper();
    Hooks::BeatmapObjectSpawnController();
    Hooks::ColorManager();
    Hooks::ColorNoteVisuals();
    Hooks::LightPairRotationEventEffect();
    Hooks::LightRotationEventEffect();
    Hooks::LightSwitchEventEffect();
    Hooks::NoteCutEffectSpawner();
    Hooks::SaberManager();
    Hooks::SceneManager_Internal();
    Hooks::StandardLevelScenesTransitionSetupDataSO();
    Hooks::ObstacleController();
    Hooks::BombNoteController();
    Hooks::ParticleSystemEventEffect();
    Hooks::TrackLaneRingsRotationEffect();
    Hooks::TrackLaneRingsRotationEffectSpawner();
}