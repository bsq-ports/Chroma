#include "Chroma.hpp"

void Chroma::InstallHooks() {
    Hooks::BeatEffectSpawner();
    Hooks::BeatmapDataTransformHelper();
    Hooks::BeatmapObjectManager();
    Hooks::BeatmapObjectSpawnController();
    Hooks::BombNoteController();
    Hooks::ColorManager();
    Hooks::ColorNoteVisuals();
    Hooks::LightPairRotationEventEffect();
    Hooks::LightRotationEventEffect();
    Hooks::LightSwitchEventEffect();
    Hooks::LightWithIdManager();
    Hooks::MirroredCubeNoteController();
    Hooks::NoteCutEffectSpawner();
    Hooks::MissionLevelScenesTransitionSetupDataSO();
    Hooks::MultiplayerLevelScenesTransitionSetupDataSO();
    Hooks::ObstacleController();
    Hooks::ParametricBoxController();
    Hooks::ParticleSystemEventEffect();
    Hooks::SaberManager();
    Hooks::SceneManager_Internal();
    Hooks::StandardLevelScenesTransitionSetupDataSO();
    Hooks::TrackLaneRing();
    Hooks::TrackLaneRingsManager();
    Hooks::TrackLaneRingsPositionStepEffectSpawner();
    Hooks::TrackLaneRingsRotationEffect();
    Hooks::TrackLaneRingsRotationEffectSpawner();
    Hooks::TutorialScenesTransitionSetupDataSO();
}