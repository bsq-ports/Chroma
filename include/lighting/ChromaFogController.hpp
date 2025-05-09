#pragma once
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

#include "GlobalNamespace/BloomFogSO.hpp"
#include "GlobalNamespace/BloomFogEnvironmentParams.hpp"

#include "UnityEngine/MonoBehaviour.hpp"

#include "ChromaController.hpp"

#include "tracks/shared/Animation/Track.h"
#include "tracks/shared/Animation/Easings.h"
#include "main.hpp"


namespace Chroma {
constexpr float fogAttenuationFix(float attenuation) {
  // if attenuation is not 0
  if (attenuation < -std::numeric_limits<float>::epsilon() || attenuation > std::numeric_limits<float>::epsilon()) {
    // clamp to minimum float fog value
    // this is a random magic number that doesn't cause the shader to jump to 0
    // TODO: Investigate, ask Split?
    return std::max(attenuation, 0.000061035154435f); // 0.0001f
  }

  return attenuation;
}
} // namespace Chroma

DECLARE_CLASS_CODEGEN(Chroma, ChromaFogController, UnityEngine::MonoBehaviour) {
private:
  static Chroma::ChromaFogController* _instance;

  TrackW _track;

  // nullable
  DECLARE_INSTANCE_FIELD(GlobalNamespace::BloomFogSO*, bloomFog);
  DECLARE_INSTANCE_FIELD(GlobalNamespace::BloomFogEnvironmentParams*, _transitionFogParams);

public:
  DECLARE_DEFAULT_CTOR();
  DECLARE_INSTANCE_METHOD(void, Awake);
  DECLARE_INSTANCE_METHOD(void, Update);
  DECLARE_INSTANCE_METHOD(void, OnDestroy);

  DECLARE_STATIC_METHOD(Chroma::ChromaFogController*, getInstance);
  DECLARE_STATIC_METHOD(void, clearInstance);
  DECLARE_SIMPLE_DTOR();

public:
  void AssignTrack(TrackW track);
};
