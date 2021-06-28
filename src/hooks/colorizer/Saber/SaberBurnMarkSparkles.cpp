#include "Chroma.hpp"

#include "ChromaController.hpp"

#include "GlobalNamespace/BaseNoteVisuals.hpp"
#include "GlobalNamespace/BombNoteController.hpp"
#include "GlobalNamespace/MirroredBombNoteController.hpp"
#include "GlobalNamespace/MultiplayerConnectedPlayerBombNoteController.hpp"
#include "GlobalNamespace/MultiplayerConnectedPlayerNoteController.hpp"
#include "GlobalNamespace/TutorialNoteController.hpp"
#include "GlobalNamespace/MirroredCubeNoteController.hpp"
#include "GlobalNamespace/SaberBurnMarkArea.hpp"

#include "UnityEngine/ParticleSystem.hpp"
#include "UnityEngine/ParticleSystem_MainModule.hpp"
#include "UnityEngine/ParticleSystem_MinMaxGradient.hpp"

#include "colorizer/SaberColorizer.hpp"
#include "utils/ChromaUtils.hpp"

using namespace GlobalNamespace;
using namespace Chroma;
using namespace UnityEngine;

int subCount = 0;

std::vector<ParticleSystem*> _burnMarksPS = {nullptr, nullptr};

void OnSaberColorChanged(int saberType, Color color)
{
    ParticleSystem::MainModule main = _burnMarksPS[(int)saberType]->get_main();
    main.set_startColor(color);
}

MAKE_HOOK_OFFSETLESS(SaberBurnMarkSparkles_Start, void, SaberBurnMarkSparkles* self) {
    SaberBurnMarkSparkles_Start(self);

    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        return;
    }

    self->burnMarksPS->copy_to(_burnMarksPS);
    if (subCount == 0) {
        SaberColorizer::SaberColorChanged += &OnSaberColorChanged;
    }
    subCount++;
}

MAKE_HOOK_OFFSETLESS(SaberBurnMarkSparkles_OnDestroy, void, SaberBurnMarkSparkles* self) {
    SaberBurnMarkSparkles_OnDestroy(self);
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        return;
    }

    subCount--;

    if (subCount == 0) {
        SaberColorizer::SaberColorChanged -= &OnSaberColorChanged;
    }
}

void Chroma::Hooks::SaberBurnMarkSparkles() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), SaberBurnMarkSparkles_Start, il2cpp_utils::FindMethodUnsafe("", "SaberBurnMarkSparkles", "Start", 0));
    INSTALL_HOOK_OFFSETLESS(getLogger(), SaberBurnMarkSparkles_OnDestroy, il2cpp_utils::FindMethodUnsafe("", "SaberBurnMarkSparkles", "OnDestroy", 0));
}