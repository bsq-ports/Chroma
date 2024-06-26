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
#include "UnityEngine/Pose.hpp"

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
  auto activeObstacleControllers = self->_beatmapObjectManager->activeObstacleControllers;
  Pose identity = Pose::get_identity();
  for (int i = 0; i < 2; i++) {
    bool flag = self->_effects[i]->IsEmitting();
    if (ObstacleSaberSparkleEffectManager::IntersectSaberWithObstacles(self->_sabers[i], activeObstacleControllers,
                                                                      byref(identity))) {
      self->_effects[i]->SetPositionAndRotation(identity.position, identity.rotation);
      // TRANSPILE IS HERE
      // TODO: Reimplement
      // SetObstacleSaberSparkleColor(self->_effects.get(i), obstacleController);
      // TRANSPILE IS HERE
      self->_hapticFeedbackManager->PlayHapticFeedback(SaberTypeExtensions::Node(self->_sabers[i]->saberType),
                                                      self->_rumblePreset);
      if (!flag) {
        self->_effects[i]->StartEmission();
        auto action = self->sparkleEffectDidStartEvent;
        if (action != nullptr) {
          action->Invoke(self->_sabers[i]->saberType);
        }
      }
    } else if (flag) {
      self->_effects[i]->StopEmission();
      auto action2 = self->sparkleEffectDidEndEvent;
      if (action2 != nullptr) {
        action2->Invoke(self->_sabers[i]->saberType);
      }
    }
  }
}

void ObstacleSaberSparkleEffectManagerHook() {
  // INSTALL_HOOK_ORIG(ChromaLogger::Logger, ObstacleSaberSparkleEffectManager_Update);
}

ChromaInstallHooks(ObstacleSaberSparkleEffectManagerHook)