#pragma once

#include "UnityEngine/ParticleSystem.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Color.hpp"

#include "GlobalNamespace/SaberType.hpp"
#include "GlobalNamespace/BasicBeatmapEventType.hpp"
#include "GlobalNamespace/ParticleSystemEventEffect.hpp"

#include "colorizer/ParticleColorizer.hpp"

#include <vector>
#include <string>
#include <optional>
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include "main.hpp"

DECLARE_CLASS_CODEGEN(Chroma, ChromaParticleEventController, UnityEngine::MonoBehaviour, private
                      : std::shared_ptr<ParticleColorizer>
                          _colorizer;
                      GlobalNamespace::BasicBeatmapEventType _eventType;

                      public
                      : DECLARE_INSTANCE_METHOD(void, Init,
                                                GlobalNamespace::ParticleSystemEventEffect* particleSystemEventEffect,
                                                GlobalNamespace::BasicBeatmapEventType eventType);
                      DECLARE_INSTANCE_METHOD(void, OnDestroy);

                      DECLARE_SIMPLE_DTOR(); DECLARE_DEFAULT_CTOR();)