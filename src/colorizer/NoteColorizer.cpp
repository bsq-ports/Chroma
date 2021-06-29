#include "Chroma.hpp"

#include "UnityEngine/SpriteRenderer.hpp"
#include "UnityEngine/MaterialPropertyBlock.hpp"
#include "UnityEngine/MeshRenderer.hpp"

#include "GlobalNamespace/GameNoteController.hpp"
#include "GlobalNamespace/MaterialPropertyBlockController.hpp"

#include "System/Action_2.hpp"

#include <unordered_map>
#include "colorizer/NoteColorizer.hpp"
#include "colorizer/SaberColorizer.hpp"
#include "ChromaController.hpp"
#include "utils/ChromaUtils.hpp"
#include "ChromaObjectData.hpp"


using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;

using namespace Chroma;

NoteColorizer::NoteColorizer(GlobalNamespace::NoteControllerBase *noteController)
:_noteController(noteController)
{

    auto colorNoteVisuals = _noteController->GetComponent<GlobalNamespace::ColorNoteVisuals*>();
    _colorNoteVisuals = colorNoteVisuals;

    colorNoteVisuals->materialPropertyBlockControllers->copy_to(_materialPropertyBlockControllers);
    _materialPropertyBlockControllers.shrink_to_fit();
}

std::shared_ptr<NoteColorizer> NoteColorizer::New(GlobalNamespace::NoteControllerBase *noteControllerBase) {
    std::shared_ptr<NoteColorizer> noteColorizer(new NoteColorizer(noteControllerBase));

    Colorizers[noteControllerBase] = noteColorizer;
    return noteColorizer;
}

std::vector<UnityEngine::Color> NoteColorizer::getOriginalColors() {
    if (!_originalColors) {
        ColorManager *colorManager = _colorNoteVisuals->colorManager;
        if (colorManager) {
            _originalColors =
                    {
                            colorManager->ColorForType(ColorType::ColorA),
                            colorManager->ColorForType(ColorType::ColorB),
                    };
        } else {
            getLogger().warning("_colorManager was null, defaulting to red/blue");
            _originalColors =
                    {
                            Color(0.784f, 0.078f, 0.078f),
                            Color(0, 0.463f, 0.823f),
                    };
        }
    }


    return *_originalColors;
}

GlobalNamespace::ColorType NoteColorizer::getColorType() {
    auto gameNoteControllerCast = il2cpp_utils::try_cast<GameNoteController>(_noteController);
    if (gameNoteControllerCast)
    {
        GameNoteController* gameNoteController = *gameNoteControllerCast;
        auto noteData = gameNoteController->noteData;
        if (noteData)
        {
            return noteData->colorType;
        }
    }

    return ColorType::ColorA;
}

std::optional<UnityEngine::Color> NoteColorizer::GlobalColorGetter() {
    return GlobalColor[(int) getColorType()];
}

std::optional<UnityEngine::Color> NoteColorizer::OriginalColorGetter() {
    return getOriginalColors()[(int) getColorType()];
}

void NoteColorizer::GlobalColorize(std::optional<UnityEngine::Color> color, GlobalNamespace::ColorType colorType) {
    GlobalColor[(int)colorType] = color;
    for (auto& valuePair : Colorizers)
    {
        valuePair.second->Refresh();
    }
}

void NoteColorizer::Reset() {
    GlobalColor[0] = std::nullopt;
    GlobalColor[1] = std::nullopt;
    Colorizers.clear();
    Colorizers = {};
}

void NoteColorizer::ColorizeSaber(GlobalNamespace::NoteController *noteController, NoteCutInfo &noteCutInfo) {
    if (ChromaController::DoColorizerSabers())
    {
        auto noteData = noteController->noteData;
        SaberType saberType = noteCutInfo.saberType;
        if ((int)noteData->colorType == (int)saberType)
        {
            SaberColorizer::GlobalColorize(saberType, GetNoteColorizer(noteController)->getColor());
        }
    }
}

void NoteColorizer::Refresh() {
    Color color = getColor();
    if (ChromaUtils::ColorEquals(color, _colorNoteVisuals->noteColor))
    {
        return;
    }

    _colorNoteVisuals->noteColor = color;
    for (auto materialPropertyBlockController : _materialPropertyBlockControllers)
    {
        if (!materialPropertyBlockController)
            continue;

        if (materialPropertyBlockController->materialPropertyBlock)
            materialPropertyBlockController->materialPropertyBlock->SetColor(_colorID(), color);

        materialPropertyBlockController->ApplyChanges();
    }
}

int NoteColorizer::_colorID() {
    static int colorID = UnityEngine::Shader::PropertyToID(il2cpp_utils::newcsstr("_Color"));

    return colorID;
}
