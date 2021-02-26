#pragma once

#include "UnityEngine/MonoBehaviour.hpp"
#include "GlobalNamespace/TrackLaneRingsRotationEffect.hpp"
#include "GlobalNamespace/TrackLaneRingsManager.hpp"
#include "GlobalNamespace/BeatmapEventType.hpp"
#include "GlobalNamespace/SimpleColorSO.hpp"
#include "GlobalNamespace/MultipliedColorSO.hpp"
#include <vector>
#include <string>
#include <optional>
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include "main.hpp"

DECLARE_CLASS_CODEGEN(Chroma, ChromaRotationEffect, Il2CppObject,
public:
        DECLARE_INSTANCE_FIELD_DEFAULT(float, RotationAngle, 0.0f);

        DECLARE_INSTANCE_FIELD_DEFAULT(float, RotationStep, 0.0f);

        DECLARE_INSTANCE_FIELD_DEFAULT(float, RotationFlexySpeed, 0.0f);

        DECLARE_INSTANCE_FIELD_DEFAULT(float, RotationPropagationSpeed, 0.0f);

        DECLARE_INSTANCE_FIELD_DEFAULT(float, ProgressPos, 0.0f);

        REGISTER_FUNCTION(ChromaRotationEffect,
                          REGISTER_FIELD(RotationAngle);
        REGISTER_FIELD(RotationStep);
        REGISTER_FIELD(RotationFlexySpeed);
        REGISTER_FIELD(RotationPropagationSpeed);
        REGISTER_FIELD(ProgressPos);

                          )
)

DECLARE_CLASS_CODEGEN(Chroma, ChromaRingsRotationEffect, GlobalNamespace::TrackLaneRingsRotationEffect,
public:
                      std::vector<ChromaRotationEffect*> _activeRingRotationEffects;
                            std::vector<ChromaRotationEffect*> _ringRotationEffectsPool;

                            DECLARE_METHOD(void, AddRingRotationEffectF, float angle, float step, float propagationSpeed, float flexySpeed);
                            DECLARE_METHOD(void, CopyValues, GlobalNamespace::TrackLaneRingsRotationEffect* trackLaneRingsRotationEffect);


        DECLARE_INSTANCE_FIELD(GlobalNamespace::TrackLaneRingsManager*, _trackLaneRingsManager);
        DECLARE_INSTANCE_FIELD(float, _startupRotationAngle);

        DECLARE_INSTANCE_FIELD_DEFAULT(float, _startupRotationStep, 10.0f);

        DECLARE_INSTANCE_FIELD_DEFAULT(int, _startupRotationPropagationSpeed, 1);
        DECLARE_INSTANCE_FIELD_DEFAULT(float, _startupRotationFlexySpeed, 0.5f);

        DECLARE_METHOD(void, AddRingRotationEffect, float angle, float step, int propagationSpeed, float flexySpeed);

        DECLARE_METHOD(void, Awake);
        DECLARE_METHOD(void, Start);
        DECLARE_METHOD(void, FixedUpdate);

        DECLARE_METHOD(ChromaRotationEffect*, SpawnRingRotationEffect);
        DECLARE_METHOD(void, RecycleRingRotationEffect, ChromaRotationEffect* ringRotationEffect);

        REGISTER_FUNCTION(ChromaRingsRotationEffect,
                getLogger().debug("Registering ChromaRingsRotationEffect!");
            REGISTER_METHOD(AddRingRotationEffect);
            REGISTER_METHOD(AddRingRotationEffectF);
            REGISTER_METHOD(CopyValues);

            REGISTER_FIELD(_trackLaneRingsManager);
            REGISTER_FIELD(_startupRotationAngle);
            REGISTER_FIELD(_startupRotationStep);
            REGISTER_FIELD(_startupRotationPropagationSpeed);
            REGISTER_FIELD(_startupRotationFlexySpeed);

            REGISTER_METHOD(Awake);
            REGISTER_METHOD(Start);
            REGISTER_METHOD(FixedUpdate);

            REGISTER_METHOD(SpawnRingRotationEffect);
            REGISTER_METHOD(RecycleRingRotationEffect);
        )
)