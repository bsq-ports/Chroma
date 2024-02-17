#include <cmath>

#include "ChromaController.hpp"

#include "GlobalNamespace/ObstacleSaberSparkleEffectManager.hpp"
#include "GlobalNamespace/ObstacleController.hpp"
#include "GlobalNamespace/ObstacleSaberSparkleEffect.hpp"
#include "GlobalNamespace/BeatmapObjectManager.hpp"
#include "GlobalNamespace/Saber.hpp"
#include "GlobalNamespace/SaberType.hpp"
#include "GlobalNamespace/SaberTypeExtensions.hpp"
#include "GlobalNamespace/HapticFeedbackManager.hpp"
#include "System/Action_1.hpp"

#include "UnityEngine/Color.hpp"

#include "UnityEngine/XR/XRNode.hpp"

#include "colorizer/ObstacleColorizer.hpp"

#include "custom-json-data/shared/VList.h"

using namespace GlobalNamespace;
using namespace Chroma;
using namespace UnityEngine;

void SetObstacleSaberSparkleColor(ObstacleSaberSparkleEffect* obstacleSaberSparkleEffect,
                                  GlobalNamespace::ObstacleController* obstacleController) {
  float h = 0;
  float s = 0;
  float _ = 0;
  Sombrero::FastColor::RGBToHSV(ObstacleColorizer::GetObstacleColorizer(obstacleController)->getColor(), h, s, _);
  obstacleSaberSparkleEffect->set_color(Sombrero::FastColor::HSVToRGB(h, s, 1));
}

MAKE_HOOK_MATCH(ObstacleSaberSparkleEffectManager_Update, &ObstacleSaberSparkleEffectManager::Update, void,
                ObstacleSaberSparkleEffectManager* self) {
  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    ObstacleSaberSparkleEffectManager_Update(self);
    return;
  }

  self->_wasSystemActive.get(0) = self->_isSystemActive.get(0);
  self->_wasSystemActive.get(1) = self->_isSystemActive.get(1);
  self->_isSystemActive.get(0) = false;
  self->_isSystemActive.get(1) = false;

  auto* obstacleControllers = self->_beatmapObjectManager->get_activeObstacleControllers();

  for (auto const& obstacleController : VList<GlobalNamespace::ObstacleController*>(obstacleControllers)) {
    if (obstacleController == nullptr) {
      continue;
    }

    auto const& bounds = obstacleController->bounds;
    for (int i = 0; i < 2; i++) {
      Vector3 vector;
      if (self->_sabers.get(i)->get_isActiveAndEnabled() &&
          self->GetBurnMarkPos(bounds, obstacleController->get_transform(), self->_sabers.get(i)->saberBladeBottomPos,
                               self->_sabers.get(i)->saberBladeTopPos, vector)) {
        self->_isSystemActive.get(i) = true;
        self->_burnMarkPositions.get(i) = vector;
        self->_effects.get(i)->SetPositionAndRotation(
            vector, self->GetEffectRotation(vector, obstacleController->get_transform(), bounds));

        // TRANSPILE IS HERE
        SetObstacleSaberSparkleColor(self->_effects.get(i), obstacleController);
        // TRANSPILE IS HERE

        self->_hapticFeedbackManager->PlayHapticFeedback(
            SaberTypeExtensions::Node(self->_sabers.get(i)->get_saberType()), self->_rumblePreset);
        if (!self->_wasSystemActive.get(i)) {
          self->_effects.get(i)->StartEmission();
          auto* action = self->sparkleEffectDidStartEvent;
          if (action != nullptr) {
            action->Invoke(self->_sabers.get(i)->get_saberType());
          }
        }
      }
    }
  }
#pragma unroll
  for (int j = 0; j < 2; j++) {
    if (!self->_isSystemActive.get(j) && self->_wasSystemActive.get(j)) {
      self->_effects.get(j)->StopEmission();
      auto* action2 = self->sparkleEffectDidEndEvent;
      if (action2 != nullptr) {
        action2->Invoke(self->_sabers.get(j)->get_saberType());
      }
    }
  }
}

void ObstacleSaberSparkleEffectManagerHook(Logger& /*logger*/) {
  INSTALL_HOOK_ORIG(getLogger(), ObstacleSaberSparkleEffectManager_Update);
}

ChromaInstallHooks(ObstacleSaberSparkleEffectManagerHook)