#include "main.hpp"
#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "environment_enhancements/ComponentInitializer.hpp"
#include "hooks/TrackLaneRingsManager.hpp"

#include "GlobalNamespace/TrackLaneRingsManager.hpp"
#include "GlobalNamespace/TrackLaneRing.hpp"

#include "Zenject/SceneDecoratorContext.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/Internal/ZenUtilInternal.hpp"

#include "custom-json-data/shared/VList.h"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace Zenject;
using namespace Zenject::Internal;

std::vector<GlobalNamespace::TrackLaneRingsManager*> TrackLaneRingsManagerHolder::RingManagers =
    std::vector<GlobalNamespace::TrackLaneRingsManager*>();

static void FindTrackLaneRingManager(UnityEngine::Transform* transform, std::vector<TrackLaneRingsManager*>& managers) {
  managers.push_back(transform->GetComponent<TrackLaneRingsManager*>());

  auto childCount = transform->GetChildCount();

  for (int i = 0; i < childCount; i++) {
    auto child = transform->GetChild(i);
    FindTrackLaneRingManager(child, managers);
  }
}

static TrackLaneRing* QueueInject(DiContainer* container, TrackLaneRing* prefab) {
  TrackLaneRing* trackLaneRing = UnityEngine::Object::Instantiate(prefab);
  auto injectables = VList<UnityEngine::MonoBehaviour*>();
  ZenUtilInternal::GetInjectableMonoBehavioursUnderGameObject(trackLaneRing->get_gameObject(), injectables);

  for (auto const& i : injectables)
    container->QueueForInject(i);

  return trackLaneRing;
}

MAKE_HOOK_MATCH(TrackLaneRingsManager_Start, &TrackLaneRingsManager::Start, void, TrackLaneRingsManager* self) {
  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    TrackLaneRingsManager_Start(self);
    return;
  }

  if (self->rings) return;

  self->rings = ArrayW<GlobalNamespace::TrackLaneRing*>(self->ringCount);
  auto transform = self->get_transform();
  Sombrero::FastVector3 forward = transform->get_forward();
  if (self->spawnAsChildren) {
    for (int i = 0; i < self->rings.size(); i++) {
      self->rings[i] = QueueInject(self->container, self->trackLaneRingPrefab);
      self->rings[i]->get_transform()->set_parent(transform);
      Sombrero::FastVector3 position = { 0.0f, 0.0f, (float)i * self->ringPositionStep };
      self->rings[i]->Init(position, { 0, 0, 0 });
    }
    return;
  }
  auto position = transform->get_position();
  for (int j = 0; j < self->rings.size(); j++) {
    self->rings[j] = QueueInject(self->container, self->trackLaneRingPrefab);
    Sombrero::FastVector3 position2 = forward * ((float)j * self->ringPositionStep);
    self->rings[j]->Init(position2, position);
  }
}

MAKE_HOOK_MATCH(SceneDecoratorContext_InitializeRings, &SceneDecoratorContext::Initialize, void,
                SceneDecoratorContext* self, Zenject::DiContainer* container) {
  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks() || self->decoratedContractName != "Environment") {
    SceneDecoratorContext_InitializeRings(self, container);
    return;
  }

  std::vector<TrackLaneRingsManager*> managers;
  for (auto const& n : self->get_gameObject()->get_scene().GetRootGameObjects()) {
    FindTrackLaneRingManager(n->get_transform(), managers);
  }

  for (auto manager : managers) {
    if (!manager) {
      continue;
    }

    manager->container = container;
    manager->Start();
  }

  SceneDecoratorContext_InitializeRings(self, container);
}

void TrackLaneRingsManagerHook(Logger& logger) {
  INSTALL_HOOK(logger, SceneDecoratorContext_InitializeRings);
  INSTALL_HOOK(logger, TrackLaneRingsManager_Start);
}

ChromaInstallHooks(TrackLaneRingsManagerHook)