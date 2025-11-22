#include "lighting/ChromaFogController.hpp"

#include "Chroma.hpp"
#include "ChromaController.hpp"
#include "utils/ChromaUtils.hpp"

#include "UnityEngine/Color.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Resources.hpp"
#include "tracks/shared/Animation/Animation.h"

DEFINE_TYPE(Chroma, ChromaFogController);

using namespace Chroma;
using namespace ChromaUtils;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Animation;

Chroma::ChromaFogController* ChromaFogController::_instance = nullptr;

Chroma::ChromaFogController* ChromaFogController::getInstance() {
  static ConstString str("Chroma_FogController");
  if (_instance == nullptr) {
    _instance = UnityEngine::GameObject::New_ctor(str)->AddComponent<ChromaFogController*>();
  }

  return _instance;
}

void Chroma::ChromaFogController::clearInstance() {
  if (_instance != nullptr) {
    UnityEngine::GameObject::Destroy(_instance);
    _instance = nullptr;
  }
}

void ChromaFogController::OnDestroy() {
  bloomFog->set_transitionFogParams(nullptr);
  Destroy(_transitionFogParams);
  _instance = nullptr;
}

void ChromaFogController::AssignTrack(TrackW track) {
  this->_track = track;
}

void ChromaFogController::Awake() {
  ArrayW<BloomFogSO*> fogs = Resources::FindObjectsOfTypeAll<BloomFogSO*>();

  if (fogs && fogs.size() > 0) {
    bloomFog = fogs.get(0);
  }

  _transitionFogParams = ScriptableObject::CreateInstance<BloomFogEnvironmentParams*>();
  BloomFogEnvironmentParams const* defaultParams = bloomFog->get_defaultForParams();
  _transitionFogParams->attenuation = defaultParams->attenuation;
  _transitionFogParams->offset = defaultParams->offset;
  _transitionFogParams->heightFogStartY = defaultParams->heightFogStartY;
  _transitionFogParams->heightFogHeight = defaultParams->heightFogHeight;
  bloomFog->set_transitionFogParams(_transitionFogParams);
}

void Chroma::ChromaFogController::Update() {
  if (!_track) {
    ChromaLogger::Logger.error("Track is null");
    CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Fog track is null!");
    return;
  }
  if (_transitionFogParams == nullptr) {
    ChromaLogger::Logger.error("ChromaFog TransitionFogParams is null");
    CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("ChromaFog TransitionFogParams is null!");
    return;
  }
  auto attenuation = _track.GetPropertyNamed(PropertyNames::Attentuation).GetFloat();
  if (attenuation) {
    auto attenuationFloat = attenuation.value();

    _transitionFogParams->attenuation = fogAttenuationFix(attenuationFloat);
  }

  auto offset = _track.GetPropertyNamed(PropertyNames::FogOffset).GetFloat();
  if (offset) {
    _transitionFogParams->offset = offset.value();
  }

  auto startY = _track.GetPropertyNamed(PropertyNames::HeightFogStartY).GetFloat();
  if (startY) {
    _transitionFogParams->heightFogStartY = startY.value();
  }

  auto height = _track.GetPropertyNamed(PropertyNames::HeightFogHeight).GetFloat();
  if (height) {
    _transitionFogParams->heightFogHeight = height.value();
  }

  bloomFog->transition = 1;
}
