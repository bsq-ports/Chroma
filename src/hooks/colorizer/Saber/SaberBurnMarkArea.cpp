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

void OnSaberColorChanged_SaberBurnMarkArea(int saberType, GlobalNamespace::SaberModelController* saberModelController, Color color)
{
    float h;
    float s;
    float _;

    Color::RGBToHSV(color, h, s, _);
    Color effectColor = Color::HSVToRGB(h, s, 1);
    int intType = saberType;
    _lineRenderers[intType]->set_startColor(effectColor);
    _lineRenderers[intType]->set_endColor(effectColor);
}

MAKE_HOOK_MATCH(SaberBurnMarkArea_Start,
                &SaberBurnMarkArea::Start,
                void, SaberBurnMarkArea* self) {
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

MAKE_HOOK_MATCH(SaberBurnMarkArea_OnDestroy,
                &SaberBurnMarkArea::OnDestroy,
                void, SaberBurnMarkArea* self) {
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

void SaberBurnMarkAreaHook(Logger& logger) {
    INSTALL_HOOK(logger, SaberBurnMarkArea_Start);
    INSTALL_HOOK(logger, SaberBurnMarkArea_OnDestroy);
}

ChromaInstallHooks(SaberBurnMarkAreaHook)