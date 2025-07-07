#include "colorizer/Monobehaviours/ChromaParticleEventController.hpp"

DEFINE_TYPE(Chroma, ChromaParticleEventController);

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;

void ChromaParticleEventController::Init(GlobalNamespace::ParticleSystemEventEffect* particleSystemEventEffect,
                                         GlobalNamespace::BasicBeatmapEventType eventType) {
  _eventType = eventType;
  _colorizer = ParticleColorizer::New(particleSystemEventEffect, eventType);
}

void ChromaParticleEventController::OnDestroy() {
  if (_colorizer) {
    ParticleColorizer::GetParticleColorizers(_eventType).erase(_colorizer);
  }
}
