#include "Chroma.hpp"

void Chroma::InstallHooks() {
    Hooks::BeatEffectSpawner();
    Hooks::BeatmapDataTransformHelper();
    Hooks::BeatmapObjectSpawnController();
    Hooks::BombNoteController();
    Hooks::ColorManager();
    Hooks::ColorNoteVisuals();
    Hooks::LightPairRotationEventEffect();
    Hooks::LightRotationEventEffect();
    Hooks::LightSwitchEventEffect();
    Hooks::NoteCutEffectSpawner();
    Hooks::ObstacleController();
    Hooks::ParticleSystemEventEffect();
    Hooks::SaberManager();
    Hooks::TrackLaneRingsManager();
    Hooks::SceneManager_Internal();
    Hooks::StandardLevelScenesTransitionSetupDataSO();
    Hooks::TrackLaneRingsRotationEffect();
    Hooks::TrackLaneRingsRotationEffectSpawner();
}