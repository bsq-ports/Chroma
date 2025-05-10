#pragma once

#include "UnityEngine/MonoBehaviour.hpp"

#include "GlobalNamespace/SaberModelController.hpp"
#include "GlobalNamespace/SaberType.hpp"
#include "GlobalNamespace/Saber.hpp"

namespace Chroma {
class SaberColorizer;
} // namespace Chroma



#include "custom-types/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(Chroma, ChromaSaberController, UnityEngine::MonoBehaviour) {
private:
  SaberColorizer* _colorizer;
  GlobalNamespace::SaberType _saberType;
  DECLARE_INSTANCE_FIELD(GlobalNamespace::SaberModelController*, _saberModelController);

public:
  DECLARE_INSTANCE_METHOD(void, Init, GlobalNamespace::Saber* saber);
  DECLARE_INSTANCE_METHOD(void, OnDestroy);

  DECLARE_SIMPLE_DTOR();
  DECLARE_DEFAULT_CTOR();
};