#pragma once

#include "UnityEngine/MonoBehaviour.hpp"
#include "GlobalNamespace/ILightWithId.hpp"
#include "GlobalNamespace/LightSwitchEventEffect.hpp"
#include "GlobalNamespace/BeatmapEventType.hpp"
#include "GlobalNamespace/SimpleColorSO.hpp"
#include "GlobalNamespace/MultipliedColorSO.hpp"
#include <vector>
#include <string>
#include <optional>

// TODO: Document properly
namespace Chroma {
    class LightColorizer {
    public:
        static void Reset(UnityEngine::MonoBehaviour *behaviour);

        static void ResetAllLightingColors();

        static void
        SetLightingColors(UnityEngine::MonoBehaviour *monoBehaviour, std::optional<UnityEngine::Color> Color0, std::optional<UnityEngine::Color> Color1,
                std::optional<UnityEngine::Color> Color0Boost = std::nullopt, std::optional<UnityEngine::Color> Color1Boost = std::nullopt);

        static void
        SetLightingColors(GlobalNamespace::BeatmapEventType *beatmapEventType, std::optional<UnityEngine::Color> Color0, std::optional<UnityEngine::Color> Color1,
                          std::optional<UnityEngine::Color> Color0Boost = std::nullopt, std::optional<UnityEngine::Color> Color1Boost = std::nullopt);

        static void SetAllLightingColors(std::optional<UnityEngine::Color> Color0, std::optional<UnityEngine::Color> Color1,
                                         std::optional<UnityEngine::Color> Color0Boost = std::nullopt, std::optional<UnityEngine::Color> Color1Boost = std::nullopt);

        static void SetActiveColors(GlobalNamespace::BeatmapEventType *lse);

        static void SetAllActiveColors();

        static void ClearLSEColorManagers();

        static void SetLastValue(UnityEngine::MonoBehaviour *monoBehaviour, int val);

        // Returns array of ILightWithId*
        static std::vector<GlobalNamespace::ILightWithId *> GetLights(GlobalNamespace::LightSwitchEventEffect *lse);

        // Returns 2d array of ILightWithId*
        static std::unordered_map<int, std::vector<GlobalNamespace::ILightWithId *>>
        GetLightsPropagationGrouped(GlobalNamespace::LightSwitchEventEffect *lse);

        static void LSEStart(UnityEngine::MonoBehaviour *monoBehaviour, GlobalNamespace::BeatmapEventType *beatmapEventType);

        class LSEColorManager {
        private:
            UnityEngine::MonoBehaviour *_lse;
            GlobalNamespace::BeatmapEventType *_type;

            UnityEngine::Color _lightColor0_Original;
            UnityEngine::Color _lightColor1_Original;
            UnityEngine::Color _lightColor0Boost_Original;
            UnityEngine::Color _lightColor1Boost_Original;

            GlobalNamespace::SimpleColorSO* _lightColor0;
            GlobalNamespace::SimpleColorSO* _lightColor1;
            GlobalNamespace::SimpleColorSO* _lightColor0Boost;
            GlobalNamespace::SimpleColorSO* _lightColor1Boost;

            GlobalNamespace::MultipliedColorSO* _mLightColor0;
            GlobalNamespace::MultipliedColorSO* _mHighlightColor0;
            GlobalNamespace::MultipliedColorSO* _mLightColor1;
            GlobalNamespace::MultipliedColorSO* _mHighlightColor1;

            GlobalNamespace::MultipliedColorSO* _mLightColor0Boost;
            GlobalNamespace::MultipliedColorSO* _mHighlightColor0Boost;
            GlobalNamespace::MultipliedColorSO* _mLightColor1Boost;
            GlobalNamespace::MultipliedColorSO* _mHighlightColor1Boost;

            bool _supportBoostColor;

            float _lastValue;

            LSEColorManager(UnityEngine::MonoBehaviour *mono, GlobalNamespace::BeatmapEventType *type);

        public:
            std::vector<GlobalNamespace::ILightWithId*> Lights = {};

            // 2d array of ILightWithId*
            std::unordered_map<int, std::vector<GlobalNamespace::ILightWithId *>> LightsPropagationGrouped;

            // TODO: Is this the proper return type?
            static std::vector<LSEColorManager *> GetLSEColorManager(GlobalNamespace::BeatmapEventType *type);

            static LSEColorManager *GetLSEColorManager(UnityEngine::MonoBehaviour *m);

            static LSEColorManager *CreateLSEColorManager(UnityEngine::MonoBehaviour *lse, GlobalNamespace::BeatmapEventType *type);

            void Reset();


            void SetLightingColors(std::optional<UnityEngine::Color> Color0, std::optional<UnityEngine::Color> Color1,
                                   std::optional<UnityEngine::Color> Color0Boost = std::nullopt, std::optional<UnityEngine::Color> Color1Boost = std::nullopt);

            void SetLastValue(int value);

            void SetActiveColors();

            void InitializeSOs(UnityEngine::MonoBehaviour *lse, const std::string& id, GlobalNamespace::SimpleColorSO *&sColorSO,
                               UnityEngine::Color& originalColor, GlobalNamespace::MultipliedColorSO *&mColorSO);
        };
    };
}