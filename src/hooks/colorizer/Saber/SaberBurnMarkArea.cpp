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

#include "UnityEngine/LineRenderer.hpp"

#include "colorizer/SaberColorizer.hpp"
#include "utils/ChromaUtils.hpp"

using namespace GlobalNamespace;
using namespace Chroma;
using namespace UnityEngine;

std::vector<LineRenderer*> _lineRenderers = {nullptr, nullptr};

int saberBurnMarkCount = 0;

void OnSaberColorChanged_SaberBurnMarkArea(int saberType, Color color)
{
    int intType = saberType;
    _lineRenderers[intType]->set_startColor(color);
    _lineRenderers[intType]->set_endColor(color);
}

MAKE_HOOK_OFFSETLESS(SaberBurnMarkArea_Start, void, SaberBurnMarkArea* self) {
    SaberBurnMarkArea_Start(self);

    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        return;
    }

    self->lineRenderers->copy_to(_lineRenderers);

    if (saberBurnMarkCount == 0) {
        SaberColorizer::SaberColorChanged += &OnSaberColorChanged_SaberBurnMarkArea;
    }
    saberBurnMarkCount++;
}

MAKE_HOOK_OFFSETLESS(SaberBurnMarkArea_OnDestroy, void, SaberBurnMarkArea* self) {
    SaberBurnMarkArea_OnDestroy(self);
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        return;
    }

    saberBurnMarkCount--;

    if (saberBurnMarkCount == 0) {
        SaberColorizer::SaberColorChanged -= &OnSaberColorChanged_SaberBurnMarkArea;
    }
}

void Chroma::Hooks::SaberBurnMarkArea() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), SaberBurnMarkArea_Start, il2cpp_utils::FindMethodUnsafe("", "SaberBurnMarkArea", "Start", 0));
    INSTALL_HOOK_OFFSETLESS(getLogger(), SaberBurnMarkArea_OnDestroy, il2cpp_utils::FindMethodUnsafe("", "SaberBurnMarkArea", "OnDestroy", 0));
}