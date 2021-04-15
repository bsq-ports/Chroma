#pragma once

#include "UnityEngine/MonoBehaviour.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/ColorNoteVisuals.hpp"
#include "GlobalNamespace/ColorManager.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"
#include "UnityEngine/Shader.hpp"
#include "ChromaObjectData.hpp"
#include <vector>
#include <string>
#include <optional>



// TODO: Document properly
// TODO: Does this need to become a custom type?
namespace Chroma {
    class NoteColorizer {
    private:
        static std::unordered_map<int ,std::optional<UnityEngine::Color>> NoteColorOverride;

    public:
        static std::optional<UnityEngine::Color> getNoteColorOverride(int color);
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
         std::shared_ptr<ChromaNoteData> _chromaData;

         GlobalNamespace::ColorNoteVisuals* _cnv;
         GlobalNamespace::NoteController* _nc;
         GlobalNamespace::ColorManager* _colorManager;
         CustomJSONData::CustomNoteData* _noteData;

         CNVColorManager(GlobalNamespace::ColorNoteVisuals* cnv, GlobalNamespace::NoteController* nc);

        public:
            static std::unordered_map<int, std::optional<UnityEngine::Color>> GlobalColor;

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

