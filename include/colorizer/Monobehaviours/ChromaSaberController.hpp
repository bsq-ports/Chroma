#pragma once

#include "UnityEngine/ParticleSystem.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Color.hpp"

#include "GlobalNamespace/SaberType.hpp"
#include "GlobalNamespace/BeatmapEventType.hpp"
#include "GlobalNamespace/Saber.hpp"

#include "colorizer/SaberColorizer.hpp"

#include <vector>
#include <string>
#include <optional>
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include "main.hpp"


DECLARE_CLASS_CODEGEN(Chroma, ChromaSaberController, UnityEngine::MonoBehaviour,
  private:
          std::shared_ptr<SaberColorizer> _colorizer;
          GlobalNamespace::SaberType _saberType;
          DECLARE_INSTANCE_FIELD(GlobalNamespace::SaberModelController*, _saberModelController);

          public:
          DECLARE_INSTANCE_METHOD(void, Init, GlobalNamespace::Saber* saber);
          DECLARE_INSTANCE_METHOD(void, OnDestroy);

          DECLARE_SIMPLE_DTOR();
          DECLARE_DEFAULT_CTOR();
)