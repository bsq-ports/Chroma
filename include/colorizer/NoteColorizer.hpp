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

typedef std::pair<std::optional<Sombrero::FastColor>, std::optional<Sombrero::FastColor>> ColorPair;
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
        std::optional<std::array<Sombrero::FastColor, 2>> _originalColors;

        NoteColorizer(GlobalNamespace::NoteControllerBase* noteController);

    protected:
//        override Color? GlobalColorGetter => GlobalColor[(int)ColorType];
//        override Color OriginalColorGetter => OriginalColors[(int)ColorType];
        std::optional<Sombrero::FastColor> GlobalColorGetter() override;

        std::optional<Sombrero::FastColor> OriginalColorGetter() override;

        void Refresh() override;

    public:
        inline static bool NoteColorable = false;
        inline static std::unordered_map<GlobalNamespace::NoteControllerBase*, std::shared_ptr<NoteColorizer>> Colorizers;
        inline static std::array<std::optional<Sombrero::FastColor>, 2> GlobalColor = {std::nullopt, std::nullopt};
        std::array<Sombrero::FastColor, 2> getOriginalColors();
        GlobalNamespace::ColorType getColorType();

        static std::shared_ptr<NoteColorizer> New(GlobalNamespace::NoteControllerBase* noteControllerBase);

        static void GlobalColorize(std::optional<Sombrero::FastColor> const& color, GlobalNamespace::ColorType const& colorType);

        static void Reset();

        static void ColorizeSaber(GlobalNamespace::NoteController* noteController, GlobalNamespace::NoteCutInfo& noteCutInfo);

        // extensions
        inline static std::shared_ptr<NoteColorizer> GetNoteColorizer(GlobalNamespace::NoteControllerBase* noteController) {
            auto it = Colorizers.find(noteController);
            if (it == Colorizers.end())
                return nullptr;

            return it->second;
        }

        inline static void ColorizeNote(GlobalNamespace::NoteControllerBase* noteController, std::optional<Sombrero::FastColor> const& color) {
            GetNoteColorizer(noteController)->Colorize(color);
        }
    };
}

