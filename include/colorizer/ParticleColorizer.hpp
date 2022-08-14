#pragma once

#include "Chroma.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/typedefs-array.hpp"

#include "lighting/LightIDTableManager.hpp"
#include "colorizer/LightColorizer.hpp"
#include "utils/ChromaUtils.hpp"

#include <unordered_map>

#include "custom-json-data/shared/CustomBeatmapData.h"

#include "UnityEngine/Mathf.hpp"

#include "GlobalNamespace/LightWithIdMonoBehaviour.hpp"
#include "GlobalNamespace/LightWithIds.hpp"
#include "GlobalNamespace/TrackLaneRingsManager.hpp"
#include "GlobalNamespace/TrackLaneRing.hpp"
#include "GlobalNamespace/ParticleSystemEventEffect.hpp"
#include "GlobalNamespace/ColorSO.hpp"
#include "GlobalNamespace/SimpleColorSO.hpp"
#include "GlobalNamespace/MultipliedColorSO.hpp"
#include "GlobalNamespace/BasicBeatmapEventType.hpp"


namespace Chroma {
    class ParticleColorizer {
    private:
        // ParticleSystemEventEffect still doesn't support boost colors!
        static const int COLOR_FIELDS = 2;

        GlobalNamespace::ParticleSystemEventEffect *_particleSystemEventEffect;
        GlobalNamespace::BasicBeatmapEventType _eventType;

        std::unordered_map<int, SafePtrUnity<GlobalNamespace::SimpleColorSO>> _simpleColorSOs;
        std::unordered_map<int, SafePtrUnity<GlobalNamespace::MultipliedColorSO>> _multipliedColorSOs;
        std::unordered_map<int, SafePtrUnity<GlobalNamespace::MultipliedColorSO>> _multipliedHighlightColorSOs;

        ParticleColorizer(GlobalNamespace::ParticleSystemEventEffect *particleSystemEventEffect,
                          GlobalNamespace::BasicBeatmapEventType BasicBeatmapEventType);

        static std::unordered_set<std::shared_ptr<ParticleColorizer>>& GetOrCreateColorizerList(GlobalNamespace::BasicBeatmapEventType eventType);

        void OnLightColorChanged(GlobalNamespace::BasicBeatmapEventType eventType, std::array<Sombrero::FastColor, 4> colors);

        void InitializeSO(const std::string& id, int index, bool highlight = false);

    public:
        static std::shared_ptr<ParticleColorizer> New(GlobalNamespace::ParticleSystemEventEffect *particleSystemEventEffect,
                                                      GlobalNamespace::BasicBeatmapEventType BasicBeatmapEventType);

        inline static std::unordered_map<int, std::unordered_set<std::shared_ptr<ParticleColorizer>>> Colorizers;

        int PreviousValue;

        void UnsubscribeEvent();

        static void Reset();

        // extensions
        inline static std::unordered_set<std::shared_ptr<ParticleColorizer>>& GetParticleColorizers(GlobalNamespace::BasicBeatmapEventType BasicBeatmapEventType) {
            return Colorizers[BasicBeatmapEventType];
        }
    };
}


