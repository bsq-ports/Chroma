#pragma once

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Shader.hpp"


#include "GlobalNamespace/NoteControllerBase.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/ColorNoteVisuals.hpp"
#include "GlobalNamespace/ColorManager.hpp"


#include <vector>
#include <string>
#include <optional>
#include <stack>
#include <tuple>

#include "ChromaObjectData.hpp"
#include "ObjectColorizer.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"

typedef std::pair<std::optional<UnityEngine::Color>, std::optional<UnityEngine::Color>> ColorPair;
using NoteColorStack = std::stack<ColorPair>;

// TODO: Document properly
namespace Chroma {
    class NoteColorizer : public ObjectColorizer
    {
    private:
        static int _colorID();

        GlobalNamespace::NoteControllerBase* _noteController;


        GlobalNamespace::ColorNoteVisuals* _colorNoteVisuals;
        std::vector<GlobalNamespace::MaterialPropertyBlockController*> _materialPropertyBlockControllers;
        std::optional<std::vector<UnityEngine::Color>> _originalColors;

        NoteColorizer(GlobalNamespace::NoteControllerBase* noteController);

    protected:
//        override Color? GlobalColorGetter => GlobalColor[(int)ColorType];
//        override Color OriginalColorGetter => OriginalColors[(int)ColorType];
        std::optional<UnityEngine::Color> GlobalColorGetter() override;

        std::optional<UnityEngine::Color> OriginalColorGetter() override;

        void Refresh() override;

    public:
        inline static std::unordered_map<GlobalNamespace::NoteControllerBase*, std::shared_ptr<NoteColorizer>> Colorizers;
        inline static std::vector<std::optional<UnityEngine::Color>> GlobalColor;
        std::vector<UnityEngine::Color> getOriginalColors();
        GlobalNamespace::ColorType getColorType();

        static std::shared_ptr<NoteColorizer> New(GlobalNamespace::NoteControllerBase* noteControllerBase);

        static void GlobalColorize(std::optional<UnityEngine::Color> color, GlobalNamespace::ColorType colorType);;

        static void Reset();

        static void ColorizeSaber(GlobalNamespace::NoteController* noteController, GlobalNamespace::NoteCutInfo& noteCutInfo);

        // extensions
        inline static std::shared_ptr<NoteColorizer> GetNoteColorizer(GlobalNamespace::NoteControllerBase* noteController) {
            if (Colorizers.find(noteController) == Colorizers.end())
                return nullptr;

            return Colorizers[noteController];
        }

        inline static void ColorizeNote(GlobalNamespace::NoteControllerBase* noteController, std::optional<UnityEngine::Color> color) {
            GetNoteColorizer(noteController)->Colorize(color);
        }
    };
}

