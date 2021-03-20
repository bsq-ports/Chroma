#pragma once

#include "main.hpp"
#include "UnityEngine/Color.hpp"
#include <optional>
#include "GlobalNamespace/ILightWithId.hpp"
#include <vector>
#include <string>


inline void PrintJSONValue(const rapidjson::Value &json) {
    #if DEBUGB == 1
        return;
    #endif
    using namespace rapidjson;

    StringBuffer sb;
    PrettyWriter<StringBuffer> writer(sb);
    json.Accept(writer);
    auto str = sb.GetString();
    getLogger().info("%s", str);
}

inline void PrintJSONValue(const rapidjson::Value* json) {
    #if DEBUGB == 1
        return;
    #endif
    using namespace rapidjson;

    StringBuffer sb;
    PrettyWriter<StringBuffer> writer(sb);
    json->Accept(writer);
    auto str = sb.GetString();
    getLogger().info("%s", str);
}

namespace Chroma {
    namespace Hooks {
        void BeatEffectSpawner();
        void BeatmapObjectSpawnController();
        void ColorManager();
        void ColorNoteVisuals();
        void LightPairRotationEventEffect();
        void LightRotationEventEffect();
        void LightSwitchEventEffect();
        void NoteCutEffectSpawner();
        void SaberManager();
        void SceneManager_Internal();
        void StandardLevelScenesTransitionSetupDataSO();
        void ObstacleController();
        void BombNoteController();
        void TrackLaneRingsRotationEffect();
        void TrackLaneRingsRotationEffectSpawner();
    }

    namespace CustomData {
        struct NoteData {
            std::optional<UnityEngine::Color> _color0;
            std::optional<UnityEngine::Color> _color1;
        };
    }

    namespace ChromaLogger {
        inline const char* ColorLightSwitch = "ColorLightSwitch";
        inline const char* EnvironmentRemoval = "EnvironmentRemoval";
        inline const char* LightColorizer = "LightColorizer";
        inline const char* TrackLaneRings = "TrackLaneRings";
        inline const char* LegacyLightColor = "LegacyLightColor";
    }

    inline const std::string DIRECTION = "_direction";
    inline const std::string ENVIRONMENTREMOVAL = "_environmentRemoval";
    inline const std::string COLOR = "_color";
    inline const std::string COUNTERSPIN = "_counterSpin";
    inline const std::string DURATION = "_duration";
    inline const std::string EASING = "_easing";
    inline const std::string STARTCOLOR = "_startColor";
    inline const std::string ENDCOLOR = "_endColor";
    inline const std::string PRECISESPEED = "_preciseSpeed";
    inline const std::string LOCKPOSITION = "_lockPosition";
    inline const std::string PROPAGATIONID = "_propID";
    inline const std::string LIGHTID = "_lightID";
    inline const std::string DISABLESPAWNEFFECT = "_disableSpawnEffect";
    inline const std::string NAMEFILTER = "_nameFilter";
    inline const std::string RESET = "_reset";
    inline const std::string STEP = "_step";
    inline const std::string SPEED = "_speed";
    inline const std::string SPEEDMULT = "_speedMult";
    inline const std::string PROPMULT = "_propMult";
    inline const std::string STEPMULT = "_stepMult";
    inline const std::string PROP = "_prop";
    inline const std::string ROTATION = "_rotation";

    void InstallHooks();
}