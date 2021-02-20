#pragma once

#include "UnityEngine/MonoBehaviour.hpp"
#include "GlobalNamespace/LightSwitchEventEffect.hpp"
#include "GlobalNamespace/BeatmapEventType.hpp"
#include "GlobalNamespace/SimpleColorSO.hpp"
#include "GlobalNamespace/MultipliedColorSO.hpp"
#include "GlobalNamespace/SaberBurnMarkArea.hpp"
#include "GlobalNamespace/SaberType.hpp"
#include "GlobalNamespace/Saber.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/ColorNoteVisuals.hpp"
#include "GlobalNamespace/ColorManager.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"
#include "UnityEngine/Shader.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include <vector>
#include <string>
#include <optional>



// TODO: Document properly
// TODO: Does this need to become a custom type?
namespace Chroma {
    class NoteColorizer {
    private:
        static std::vector<std::optional<UnityEngine::Color>> NoteColorOverride;

    public:
     static void Reset(GlobalNamespace::NoteController* nc);
     static void ResetAllNotesColors();
     static void SetNoteColors(GlobalNamespace::NoteController* cnv, std::optional<UnityEngine::Color> color0, std::optional<UnityEngine::Color> color1);
     static void SetAllNoteColors(std::optional<UnityEngine::Color> color0, std::optional<UnityEngine::Color> color1);
     static void SetActiveColors(GlobalNamespace::NoteController* nc);
     static void SetAllActiveColors();
     static void ClearCNVColorManagers();
     static void EnableNoteColorOverride(GlobalNamespace::NoteController* noteController);
     static void DisableNoteColorOverride();
     static void ColorizeSaber(GlobalNamespace::NoteController* noteController, GlobalNamespace::NoteCutInfo* noteCutInfo);
    /*
     * CNV ColorSO holders
     */
    static void CNVStart(GlobalNamespace::ColorNoteVisuals* cnv, GlobalNamespace::NoteController* nc);

    class CNVColorManager
        {
        private:
         inline static int _colorID = -1;
         static std::vector<std::optional<UnityEngine::Color>> _globalColor;
         GlobalNamespace::ColorNoteVisuals* _cnv;
         GlobalNamespace::NoteController* _nc;
         GlobalNamespace::ColorManager* _colorManager;
         CustomJSONData::CustomNoteData* _noteData;

         CNVColorManager(GlobalNamespace::ColorNoteVisuals* cnv, GlobalNamespace::NoteController* nc);

        public:
            static CNVColorManager* GetCNVColorManager(GlobalNamespace::NoteController* nc);

            static CNVColorManager* CreateCNVColorManager(GlobalNamespace::ColorNoteVisuals* cnv, GlobalNamespace::NoteController* nc);

            static void SetGlobalNoteColors(std::optional<UnityEngine::Color> color0, std::optional<UnityEngine::Color> color1);

            static void ResetGlobal();

            void Reset();

            void SetNoteColors(std::optional<UnityEngine::Color> color0, std::optional<UnityEngine::Color> color1);

            UnityEngine::Color ColorForCNVManager();

            void SetActiveColors();
    };
    private:
        inline static std::vector<CNVColorManager*> _cnvColorManagers = {};
    };
}

