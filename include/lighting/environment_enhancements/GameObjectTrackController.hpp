#pragma once

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/MonoBehaviour.hpp"

#include "GlobalNamespace/TrackLaneRing.hpp"
#include "GlobalNamespace/ParametricBoxController.hpp"
#include "GlobalNamespace/BeatmapObjectsAvoidance.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"

#include "sombrero/shared/ColorUtils.hpp"

#include "tracks/shared/Animation/Track.h"

#include "custom-json-data/shared/JSONWrapper.h"
#include "custom-json-data/shared/CustomBeatmapData.h"

#include <vector>
#include <string>
#include <optional>
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include "main.hpp"


DECLARE_CLASS_CODEGEN(Chroma, GameObjectTrackController, UnityEngine::MonoBehaviour,
private:
    Track* _track;
    DECLARE_INSTANCE_FIELD(float, _noteLinesDistance);

    // nullable
    std::optional<GlobalNamespace::TrackLaneRing*> _trackLaneRing;
    std::optional<GlobalNamespace::ParametricBoxController*> _parametricBoxController;
    std::optional<GlobalNamespace::BeatmapObjectsAvoidance*> _beatmapObjectsAvoidance;

    static void HandleTrackData(UnityEngine::GameObject* gameObject,
                          rapidjson::Value& gameObjectData,
                          CustomJSONData::CustomBeatmapData* beatmapData,
                          float noteLinesDistance,
                          std::optional<GlobalNamespace::TrackLaneRing*> trackLaneRing,
                          std::optional<GlobalNamespace::ParametricBoxController*> parametricBoxController,
                          std::optional<GlobalNamespace::BeatmapObjectsAvoidance*> beatmapObjectsAvoidance);
    DECLARE_INSTANCE_METHOD(void, Update);
public:
    void Init(Track* track, float noteLinesDistance, std::optional<GlobalNamespace::TrackLaneRing*> trackLaneRing,
                            std::optional<GlobalNamespace::ParametricBoxController*> parametricBoxController,
                            std::optional<GlobalNamespace::BeatmapObjectsAvoidance*> beatmapObjectsAvoidance);



    DECLARE_SIMPLE_DTOR();
    DECLARE_DEFAULT_CTOR();
)
