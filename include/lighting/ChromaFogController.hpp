#pragma once
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

#include "GlobalNamespace/BloomFogSO.hpp"
#include "GlobalNamespace/BloomFogEnvironmentParams.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Color.hpp"

#include "ChromaController.hpp"

#include "tracks/shared/Animation/Track.h"
#include "tracks/shared/Animation/Easings.h"
#include "main.hpp"

DECLARE_CLASS_CODEGEN(Chroma, ChromaFogController, UnityEngine::MonoBehaviour,
    private:
        static Chroma::ChromaFogController* _instance;

        Track* _track;

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
        void AssignTrack(Track *track);
)
