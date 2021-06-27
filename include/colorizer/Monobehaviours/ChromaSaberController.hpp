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

          public:
          DECLARE_METHOD(void, Init, GlobalNamespace::Saber* saber);
          DECLARE_METHOD(void, OnDestroy);

          DECLARE_SIMPLE_DTOR();
          DECLARE_DEFAULT_CTOR();

          REGISTER_FUNCTION(
              REGISTER_METHOD(Init);
              REGISTER_METHOD(OnDestroy);
              REGISTER_DEFAULT_CTOR();
              REGISTER_SIMPLE_DTOR();
          )
)