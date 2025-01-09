#include <cmath>

#include "Chroma.hpp"

#include "ChromaController.hpp"

#include "GlobalNamespace/BaseNoteVisuals.hpp"
#include "GlobalNamespace/BombNoteController.hpp"
#include "GlobalNamespace/MirroredBombNoteController.hpp"
#include "GlobalNamespace/MultiplayerConnectedPlayerBombNoteController.hpp"
#include "GlobalNamespace/MultiplayerConnectedPlayerNoteController.hpp"
#include "GlobalNamespace/TutorialNoteController.hpp"
#include "GlobalNamespace/SaberBurnMarkArea.hpp"

#include "UnityEngine/LineRenderer.hpp"

#include "colorizer/SaberColorizer.hpp"
#include "utils/ChromaUtils.hpp"

using namespace GlobalNamespace;
using namespace Chroma;
using namespace UnityEngine;

std::array<LineRenderer*, 2> lineRenderers = { nullptr, nullptr };

int saberBurnMarkCount = 0;

void OnSaberColorChanged_SaberBurnMarkArea(int saberType,
                                           GlobalNamespace::SaberModelController* /*saberModelController*/,
                                           Color const& color) {
  float h = NAN;
  float s = NAN;
  float _ = NAN;

  Sombrero::FastColor::RGBToHSV(color, h, s, _);
  Sombrero::FastColor effectColor = Sombrero::FastColor::HSVToRGB(h, s, 1);
  int intType = saberType;
  if (lineRenderers[intType]) {
    lineRenderers[intType]->set_startColor(effectColor);
    lineRenderers[intType]->set_endColor(effectColor);
  }
}

MAKE_HOOK_MATCH(SaberBurnMarkArea_Start, &SaberBurnMarkArea::Start, void, SaberBurnMarkArea* self) {
  SaberBurnMarkArea_Start(self);

  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks() || (self == nullptr)) {
    return;
  }

  if (self->_lineRenderers && self->_lineRenderers.size() >= 2) {
    lineRenderers[0] = self->_lineRenderers.get(0);
    lineRenderers[1] = self->_lineRenderers.get(1);
  }

  if (saberBurnMarkCount == 0) {
    SaberColorizer::SaberColorChanged += &OnSaberColorChanged_SaberBurnMarkArea;
  }
  saberBurnMarkCount++;
}

MAKE_HOOK_MATCH(SaberBurnMarkArea_OnDestroy, &SaberBurnMarkArea::OnDestroy, void, SaberBurnMarkArea* self) {
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

void SaberBurnMarkAreaHook() {
  INSTALL_HOOK(ChromaLogger::Logger, SaberBurnMarkArea_Start);
  INSTALL_HOOK(ChromaLogger::Logger, SaberBurnMarkArea_OnDestroy);
}

ChromaInstallHooks(SaberBurnMarkAreaHook)