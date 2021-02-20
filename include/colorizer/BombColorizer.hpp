#pragma once

#include "UnityEngine/MonoBehaviour.hpp"
#include "GlobalNamespace/LightSwitchEventEffect.hpp"
#include "GlobalNamespace/BeatmapEventType.hpp"
#include "GlobalNamespace/SimpleColorSO.hpp"
#include "GlobalNamespace/MultipliedColorSO.hpp"
#include "GlobalNamespace/SaberBurnMarkArea.hpp"
#include "GlobalNamespace/SaberType.hpp"
#include "GlobalNamespace/Saber.hpp"
#include "GlobalNamespace/BombNoteController.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/ColorNoteVisuals.hpp"
#include "GlobalNamespace/ColorManager.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"
#include "UnityEngine/Material.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include <vector>
#include <string>
#include <optional>



// TODO: Document properly
// TODO: Does this need to become a custom type?
namespace Chroma {
    class NoteColorizer {
    public:
        static void Reset(GlobalNamespace::BombNoteController* bnc);
        static void ResetAllBombColors();
        static void SetBombColor(GlobalNamespace::BombNoteController* bnc, std::optional<UnityEngine::Color> color);
        static void SetAllBombColors(std::optional<UnityEngine::Color> color);
        static void ClearBNCColorManagers();

        /*
         * NC ColorSO holders
         */
        static void BNCStart(GlobalNamespace::BombNoteController* bnc);

        class BNCColorManager
        {
        private:
            static std::optional<UnityEngine::Color> _globalColor;
            GlobalNamespace::BombNoteController* _nc;
            UnityEngine::Color _color_Original;
            UnityEngine::Material* _bombMaterial;
            explicit BNCColorManager(GlobalNamespace::BombNoteController* nc);

        public:
            static BNCColorManager* GetBNCColorManager(GlobalNamespace::BombNoteController* nc);

            static BNCColorManager* CreateBNCColorManager(GlobalNamespace::BombNoteController* nc);

            static void SetGlobalBombColor(std::optional<UnityEngine::Color> color);

            static void ResetGlobal();

            void Reset();

            void SetBombColor(std::optional<UnityEngine::Color> color);
        };
    private:
        inline static std::vector<BNCColorManager*> _bncColorManagers = {};
    };
}

