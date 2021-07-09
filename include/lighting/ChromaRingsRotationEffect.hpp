#pragma once

#include "UnityEngine/MonoBehaviour.hpp"
#include "GlobalNamespace/TrackLaneRingsRotationEffect.hpp"
#include "GlobalNamespace/TrackLaneRingsManager.hpp"
#include "GlobalNamespace/BeatmapEventType.hpp"
#include "GlobalNamespace/SimpleColorSO.hpp"
#include "GlobalNamespace/MultipliedColorSO.hpp"
#include "GlobalNamespace/TrackLaneRingsManager.hpp"
#include <vector>
#include <string>
#include <optional>
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include "main.hpp"

DECLARE_CLASS_CODEGEN(Chroma, ChromaRotationEffect, Il2CppObject,
public:
        DECLARE_INSTANCE_FIELD(float, RotationAngle);

        DECLARE_INSTANCE_FIELD(float, RotationStep);

        DECLARE_INSTANCE_FIELD(float, RotationFlexySpeed);

        DECLARE_INSTANCE_FIELD(float, RotationPropagationSpeed);

        DECLARE_INSTANCE_FIELD(float, ProgressPos);
)

#define GET_FIND_METHOD(mPtr) il2cpp_utils::il2cpp_type_check::MetadataGetter<mPtr>::get()

DECLARE_CLASS_CODEGEN(Chroma, ChromaRingsRotationEffect, GlobalNamespace::TrackLaneRingsRotationEffect,
public:
        std::vector<ChromaRotationEffect*> _activeRingRotationEffects;
        std::vector<ChromaRotationEffect*> _ringRotationEffectsPool;


        DECLARE_INSTANCE_FIELD(GlobalNamespace::TrackLaneRingsManager*, _trackLaneRingsManager);
        DECLARE_INSTANCE_FIELD_DEFAULT(float, _startupRotationAngle, 10.0f);

        DECLARE_INSTANCE_FIELD(float, _startupRotationStep);

        DECLARE_INSTANCE_FIELD_DEFAULT(int, _startupRotationPropagationSpeed, 1);
        DECLARE_INSTANCE_FIELD_DEFAULT(float, _startupRotationFlexySpeed, 0.5f);

        DECLARE_CTOR(ctor);
        DECLARE_SIMPLE_DTOR();
        DECLARE_INSTANCE_METHOD(void, CopyValues, GlobalNamespace::TrackLaneRingsRotationEffect* trackLaneRingsRotationEffect);

        DECLARE_INSTANCE_METHOD(float, GetFirstRingRotationAngle);
        DECLARE_INSTANCE_METHOD(float, GetFirstRingDestinationRotationAngle);

        DECLARE_INSTANCE_METHOD(void, AddRingRotationEffectF, float angle, float step, float propagationSpeed, float flexySpeed);
        DECLARE_OVERRIDE_METHOD(void, AddRingRotationEffect, GET_FIND_METHOD(&GlobalNamespace::TrackLaneRingsRotationEffect::AddRingRotationEffect), float angle, float step, int propagationSpeed, float flexySpeed);

        DECLARE_INSTANCE_METHOD(void, SetNewRingManager, GlobalNamespace::TrackLaneRingsManager* trackLaneRingsManager);

        DECLARE_INSTANCE_METHOD(void, Awake);
        DECLARE_INSTANCE_METHOD(void, Start);
        DECLARE_INSTANCE_METHOD(void, FixedUpdate);

        DECLARE_INSTANCE_METHOD(ChromaRotationEffect*, SpawnRingRotationEffect);
        DECLARE_INSTANCE_METHOD(void, RecycleRingRotationEffect, ChromaRotationEffect* ringRotationEffect);
)