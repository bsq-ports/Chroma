#pragma once

#include "main.hpp"
#include "UnityEngine/Color.hpp"
#include <optional>
#include "GlobalNamespace/ILightWithId.hpp"
#include <vector>
#include <string>

namespace Chroma {
    namespace Hooks {
        void BeatmapObjectSpawnController();
        void LightPairRotationEventEffect();
        void LightRotationEventEffect();
        void LightSwitchEventEffect();
    }

    namespace CustomData {
        struct NoteData {
            std::optional<UnityEngine::Color> _color0;
            std::optional<UnityEngine::Color> _color1;
        };
    }

    inline const std::string DIRECTION = "_direction";
    inline const std::string ENVIRONMENTREMOVAL = "_environmentRemoval";
    inline const std::string COLOR = "_color";
    inline const std::string DURATION = "_duration";
    inline const std::string EASING = "_easing";
    inline const std::string STARTCOLOR = "_startColor";
    inline const std::string ENDCOLOR = "_endColor";
    inline const std::string PRECISESPEED = "_preciseSpeed";
    inline const std::string LOCKPOSITION = "_lockPosition";
    inline const std::string PROPAGATIONID = "_propID";
    inline const std::string LIGHTID = "_lightID";

    void InstallHooks();
}