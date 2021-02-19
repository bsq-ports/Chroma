#pragma once

#include "UnityEngine/MonoBehaviour.hpp"
#include "GlobalNamespace/ILightWithId.hpp"
#include "GlobalNamespace/LightSwitchEventEffect.hpp"
#include "GlobalNamespace/BeatmapEventType.hpp"
#include "GlobalNamespace/SimpleColorSO.hpp"
#include "GlobalNamespace/MultipliedColorSO.hpp"
#include <vector>
#include <string>

using namespace GlobalNamespace;
using namespace UnityEngine;

// TODO: Document properly
namespace Chroma {
    class LightColorizer {
    public:
        static void Reset(MonoBehaviour* behaviour);
        static void ResetAllLightingColors();

        static void SetLightingColors(MonoBehaviour* monoBehaviour, UnityEngine::Color* Color0, UnityEngine::Color* Color1, UnityEngine::Color* Color0Boost = nullptr, UnityEngine::Color* Color1Boost = nullptr);
        static void SetLightingColors(BeatmapEventType* beatmapEventType, UnityEngine::Color* Color0, UnityEngine::Color* Color1, UnityEngine::Color* Color0Boost = nullptr, UnityEngine::Color* Color1Boost = nullptr);
        static void SetAllLightingColors(UnityEngine::Color* Color0, UnityEngine::Color* Color1, UnityEngine::Color* Color0Boost = nullptr, UnityEngine::Color* Color1Boost = nullptr);
        static void SetActiveColors(BeatmapEventType* lse);
        static void SetAllActiveColors();

        static void ClearLSEColorManagers();
        static void SetLastValue(MonoBehaviour* monoBehaviour, int val);

        // Returns array of ILightWithId*
        static std::vector<ILightWithId *> GetLights(LightSwitchEventEffect* lse);

        // Returns 2d array of ILightWithId*
        static std::unordered_map<int, std::vector<ILightWithId *>> GetLightsPropagationGrouped(LightSwitchEventEffect* lse);

        static void LSEStart(MonoBehaviour* monoBehaviour, BeatmapEventType* beatmapEventType);

    private:
        class LSEColorManager {
        private:
        MonoBehaviour* _lse;
        BeatmapEventType* _type;

         UnityEngine::Color* _lightColor0_Original;
         UnityEngine::Color* _lightColor1_Original;
         UnityEngine::Color* _lightColor0Boost_Original;
         UnityEngine::Color* _lightColor1Boost_Original;

         SimpleColorSO* _lightColor0;
         SimpleColorSO* _lightColor1;
         SimpleColorSO* _lightColor0Boost;
         SimpleColorSO* _lightColor1Boost;

         MultipliedColorSO* _mLightColor0;
         MultipliedColorSO* _mHighlightColor0;
         MultipliedColorSO* _mLightColor1;
         MultipliedColorSO* _mHighlightColor1;

         MultipliedColorSO* _mLightColor0Boost;
         MultipliedColorSO* _mHighlightColor0Boost;
         MultipliedColorSO* _mLightColor1Boost;
         MultipliedColorSO* _mHighlightColor1Boost;

         bool _supportBoostColor;

         float _lastValue;

         LSEColorManager(MonoBehaviour* mono, BeatmapEventType* type);

        public:
            std::vector<ILightWithId*> Lights;

            // 2d array of ILightWithId*
            std::unordered_map<int, std::vector<ILightWithId *>> LightsPropagationGrouped;

            // TODO: Is this the proper return type?
            static std::vector<LSEColorManager*> GetLSEColorManager(BeatmapEventType* type);

            static LSEColorManager* GetLSEColorManager(MonoBehaviour* m);

            static LSEColorManager* CreateLSEColorManager(MonoBehaviour* lse, BeatmapEventType* type);

            void Reset();


            void SetLightingColors(UnityEngine::Color* Color0, UnityEngine::Color* Color1, UnityEngine::Color* Color0Boost = nullptr, UnityEngine::Color* Color1Boost = nullptr);

            void SetLastValue(int value);

            void SetActiveColors();

            void InitializeSOs(MonoBehaviour* lse, std::string id, SimpleColorSO* sColorSO, UnityEngine::Color* originalColor, MultipliedColorSO* mColorSO);
        };

        // TODO: Is this the proper equivalent of HashSet<LSEColorManager>?
        static std::vector<LSEColorManager*> _lseColorManagers;
    };
}