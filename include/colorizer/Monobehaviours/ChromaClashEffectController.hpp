#pragma once

#include "UnityEngine/ParticleSystem.hpp"
#include "UnityEngine/MonoBehaviour.hpp"

#include "GlobalNamespace/SaberModelController.hpp"
#include "GlobalNamespace/ColorManager.hpp"
#include "GlobalNamespace/SaberType.hpp"

#include "sombrero/shared/ColorUtils.hpp"

#include <vector>
#include <string>
#include <optional>
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include "main.hpp"

DECLARE_CLASS_CODEGEN(Chroma, ChromaClashEffectController, UnityEngine::MonoBehaviour) {
private:
  DECLARE_INSTANCE_FIELD(UnityEngine::ParticleSystem*, _sparkleParticleSystem);
  DECLARE_INSTANCE_FIELD(UnityEngine::ParticleSystem*, _glowParticleSystem);
  std::array<Sombrero::FastColor, 2> _colors;

  void OnSaberColorChanged(int saberType, GlobalNamespace::SaberModelController* saberModelController,
                           Sombrero::FastColor const& color);

public:
  DECLARE_INSTANCE_METHOD(void, Init, UnityEngine::ParticleSystem* sparkleParticleSystem,
                          UnityEngine::ParticleSystem* glowParticleSystem, GlobalNamespace::ColorManager* colorManager);
  DECLARE_INSTANCE_METHOD(void, OnDestroy);

  DECLARE_SIMPLE_DTOR();
  DECLARE_DEFAULT_CTOR();
};
