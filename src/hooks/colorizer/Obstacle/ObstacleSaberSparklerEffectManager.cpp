#include <cmath>

#include "ChromaController.hpp"

#include "GlobalNamespace/ObstacleSaberSparkleEffectManager.hpp"
#include "GlobalNamespace/ObstacleController.hpp"
#include "GlobalNamespace/ObstacleSaberSparkleEffect.hpp"
#include "GlobalNamespace/BeatmapObjectManager.hpp"
#include "GlobalNamespace/Saber.hpp"
#include "GlobalNamespace/SaberType.hpp"
#include "GlobalNamespace/SaberTypeExtensions.hpp"
#include "GlobalNamespace/HapticFeedbackController.hpp"
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

  self->wasSystemActive.get(0) = self->isSystemActive.get(0);
  self->wasSystemActive.get(1) = self->isSystemActive.get(1);
  self->isSystemActive.get(0) = false;
  self->isSystemActive.get(1) = false;

  auto* obstacleControllers = self->beatmapObjectManager->get_activeObstacleControllers();

  for (auto const& obstacleController : VList<GlobalNamespace::ObstacleController*>(obstacleControllers)) {
    if (obstacleController == nullptr) {
      continue;
    }

    auto const& bounds = obstacleController->bounds;
    for (int i = 0; i < 2; i++) {
      Vector3 vector;
      if (self->sabers.get(i)->get_isActiveAndEnabled() &&
          self->GetBurnMarkPos(bounds, obstacleController->get_transform(), self->sabers.get(i)->saberBladeBottomPos,
                               self->sabers.get(i)->saberBladeTopPos, vector)) {
        self->isSystemActive.get(i) = true;
        self->burnMarkPositions.get(i) = vector;
        self->effects.get(i)->SetPositionAndRotation(
            vector, self->GetEffectRotation(vector, obstacleController->get_transform(), bounds));

        // TRANSPILE IS HERE
        SetObstacleSaberSparkleColor(self->effects.get(i), obstacleController);
        // TRANSPILE IS HERE

        self->hapticFeedbackController->PlayHapticFeedback(
            SaberTypeExtensions::Node(self->sabers.get(i)->get_saberType()), self->rumblePreset);
        if (!self->wasSystemActive.get(i)) {
          self->effects.get(i)->StartEmission();
          auto* action = self->sparkleEffectDidStartEvent;
          if (action != nullptr) {
            action->Invoke(self->sabers.get(i)->get_saberType());
          }
        }
      }
    }
  }
#pragma unroll
  for (int j = 0; j < 2; j++) {
    if (!self->isSystemActive.get(j) && self->wasSystemActive.get(j)) {
      self->effects.get(j)->StopEmission();
      auto* action2 = self->sparkleEffectDidEndEvent;
      if (action2 != nullptr) {
        action2->Invoke(self->sabers.get(j)->get_saberType());
      }
    }
  }
}

void ObstacleSaberSparkleEffectManagerHook(Logger& /*logger*/) {
  INSTALL_HOOK_ORIG(getLogger(), ObstacleSaberSparkleEffectManager_Update);
}

ChromaInstallHooks(ObstacleSaberSparkleEffectManagerHook)