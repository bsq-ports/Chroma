#include "Chroma.hpp"

#include "ChromaController.hpp"

#include "GlobalNamespace/BaseNoteVisuals.hpp"
#include "GlobalNamespace/BombNoteController.hpp"
#include "GlobalNamespace/MirroredBombNoteController.hpp"
#include "GlobalNamespace/MultiplayerConnectedPlayerBombNoteController.hpp"
#include "GlobalNamespace/MultiplayerConnectedPlayerNoteController.hpp"
#include "GlobalNamespace/TutorialNoteController.hpp"
#include "GlobalNamespace/SaberClashEffect.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/ParticleSystem.hpp"

#include "colorizer/Monobehaviours/ChromaClashEffectController.hpp"
#include "colorizer/SaberColorizer.hpp"
#include "utils/ChromaUtils.hpp"

using namespace GlobalNamespace;
using namespace Chroma;
using namespace UnityEngine;

MAKE_HOOK_MATCH(SaberClashEffect_Start, &SaberClashEffect::Start, void, SaberClashEffect* self) {
  SaberClashEffect_Start(self);

  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    return;
  }

  self->get_gameObject()->AddComponent<ChromaClashEffectController*>()->Init(
      self->_sparkleParticleSystem, self->_glowParticleSystem, self->_colorManager);
}

void SaberClashEffect() {
  INSTALL_HOOK(getLogger(), SaberClashEffect_Start);
}