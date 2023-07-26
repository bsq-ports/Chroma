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

#include "UnityEngine/ParticleSystem.hpp"
#include "UnityEngine/ParticleSystem_MainModule.hpp"
#include "UnityEngine/ParticleSystem_MinMaxGradient.hpp"

#include "colorizer/SaberColorizer.hpp"
#include "utils/ChromaUtils.hpp"

using namespace GlobalNamespace;
using namespace Chroma;
using namespace UnityEngine;

int subCount = 0;

std::array<ParticleSystem*, 2> burnMarksPS = { nullptr, nullptr };

void OnSaberColorChanged(int saberType, GlobalNamespace::SaberModelController* /*saberModelController*/,
                         Color const& color) {
  float h = NAN;
  float s = NAN;
  float _ = NAN;

  Sombrero::FastColor::RGBToHSV(color, h, s, _);
  Sombrero::FastColor effectColor = Sombrero::FastColor::HSVToRGB(h, s, 1);
  ParticleSystem::MainModule main = burnMarksPS[saberType]->get_main();
  main.set_startColor(effectColor);
}

MAKE_HOOK_MATCH(SaberBurnMarkSparkles_Start, &SaberBurnMarkSparkles::Start, void, SaberBurnMarkSparkles* self) {
  SaberBurnMarkSparkles_Start(self);

  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks() || (self == nullptr)) {
    return;
  }
  if (self->burnMarksPS && self->burnMarksPS.Length() >= 2) {
    burnMarksPS[0] = self->burnMarksPS.get(0);
    burnMarksPS[1] = self->burnMarksPS.get(1);
  }

  if (subCount == 0) {
    SaberColorizer::SaberColorChanged += &OnSaberColorChanged;
  }
  subCount++;
}

MAKE_HOOK_MATCH(SaberBurnMarkSparkles_OnDestroy, &SaberBurnMarkSparkles::OnDestroy, void, SaberBurnMarkSparkles* self) {
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

void SaberBurnMarkSparklesHook(Logger& logger) {
  INSTALL_HOOK(logger, SaberBurnMarkSparkles_Start);
  INSTALL_HOOK(logger, SaberBurnMarkSparkles_OnDestroy);
}

ChromaInstallHooks(SaberBurnMarkSparklesHook)