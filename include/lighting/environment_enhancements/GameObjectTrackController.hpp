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

DECLARE_CLASS_CODEGEN(Chroma, GameObjectTrackControllerData, Il2CppObject,
public:
        Track* _track;

        // nullable
        std::optional<GlobalNamespace::TrackLaneRing*> _trackLaneRing;
        std::optional<GlobalNamespace::ParametricBoxController*> _parametricBoxController;
        std::optional<GlobalNamespace::BeatmapObjectsAvoidance*> _beatmapObjectsAvoidance;

        DECLARE_INSTANCE_FIELD(float, _noteLinesDistance);
)


DECLARE_CLASS_CODEGEN(Chroma, GameObjectTrackController, UnityEngine::MonoBehaviour,

private:
    inline static int nextId = 0;

    // Unity doesn't like copying my data, so we store it and copy the ID.
    inline static std::unordered_map<int, GameObjectTrackControllerData*> _dataMap{};

    DECLARE_INSTANCE_FIELD(int, id);

    // This is retrived from the data map since Unity doesn't copy it.
    GameObjectTrackControllerData* data;

    DECLARE_INSTANCE_METHOD(void, Update);
    DECLARE_CTOR(ctor);
public:
    void Init(Track* track, float noteLinesDistance, std::optional<GlobalNamespace::TrackLaneRing*> trackLaneRing,
                            std::optional<GlobalNamespace::ParametricBoxController*> parametricBoxController,
                            std::optional<GlobalNamespace::BeatmapObjectsAvoidance*> beatmapObjectsAvoidance);

    static void HandleTrackData(UnityEngine::GameObject* gameObject,
                                std::optional<Track*> track,
                                float noteLinesDistance,
                                std::optional<GlobalNamespace::TrackLaneRing*> trackLaneRing,
                                std::optional<GlobalNamespace::ParametricBoxController*> parametricBoxController,
                                std::optional<GlobalNamespace::BeatmapObjectsAvoidance*> beatmapObjectsAvoidance);

    static void ClearData();

    DECLARE_SIMPLE_DTOR();
)
