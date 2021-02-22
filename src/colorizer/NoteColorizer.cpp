#include "Chroma.hpp"

#include "UnityEngine/SpriteRenderer.hpp"
#include "UnityEngine/MaterialPropertyBlock.hpp"
#include "GlobalNamespace/MaterialPropertyBlockController.hpp"

#include <unordered_map>
#include "colorizer/NoteColorizer.hpp"
#include "colorizer/SaberColorizer.hpp"
#include "ChromaController.hpp"


using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;

using namespace Chroma;
//
//std::vector<std::optional<UnityEngine::Color>> NoteColorizer::SaberColorOverride = std::vector<std::optional<UnityEngine::Color>>(2);
//
//std::vector<SaberColorizer::BSMColorManager *> SaberColorizer::_bsmColorManagers;

std::vector<std::optional<UnityEngine::Color>> NoteColorizer::NoteColorOverride = std::vector<std::optional<UnityEngine::Color>>(2);
std::vector<std::optional<UnityEngine::Color>> NoteColorizer::CNVColorManager::_globalColor = std::vector<std::optional<UnityEngine::Color>>(2);

void NoteColorizer::Reset(GlobalNamespace::NoteController *nc) {
    auto m = CNVColorManager::GetCNVColorManager(nc);

    if (m)
        m->Reset();
}

void NoteColorizer::ResetAllNotesColors() {
    CNVColorManager::ResetGlobal();

    for (auto cnvColorManager : _cnvColorManagers)
    {
        cnvColorManager->Reset();
    }
}

void NoteColorizer::SetNoteColors(GlobalNamespace::NoteController *cnv, std::optional<UnityEngine::Color> color0,
                                  std::optional<UnityEngine::Color> color1) {
    auto m = CNVColorManager::GetCNVColorManager(cnv);

    if (m)
        m->SetNoteColors(color0, color1);
}

void
NoteColorizer::SetAllNoteColors(std::optional<UnityEngine::Color> color0, std::optional<UnityEngine::Color> color1) {
    CNVColorManager::SetGlobalNoteColors(color0, color1);

    for (auto cnvColorManager : _cnvColorManagers)
    {
        cnvColorManager->Reset();
    }
}

void NoteColorizer::SetActiveColors(GlobalNamespace::NoteController *nc) {
    CNVColorManager::GetCNVColorManager(nc)->SetActiveColors();
}

void NoteColorizer::SetAllActiveColors() {
    for (auto cnvColorManager : _cnvColorManagers)
    {
        cnvColorManager->SetActiveColors();
    }
}

void NoteColorizer::ClearCNVColorManagers() {
    ResetAllNotesColors();
    _cnvColorManagers.clear();
}

void NoteColorizer::EnableNoteColorOverride(GlobalNamespace::NoteController *noteController) {
    if (il2cpp_functions::class_is_assignable_from(noteController->noteData->klass, classof(CustomNoteData*))) {
        auto *customData = reinterpret_cast<CustomNoteData *>(noteController->noteData);
        auto dynData = customData->customData;

        CustomData::NoteData* noteData = static_cast<CustomData::NoteData *>(dynData->associatedData['C']);

        // TODO: Do these execute a similar or exact implementation of the PC version at
        // https://github.com/Aeroluna/Chroma/blob/e7a72f8b848c822d860361a027034218125af135/Chroma/Colorizer/NoteColorizer.cs#L71-L72

        NoteColorOverride[0] = noteData->_color0; //.at(dynData, "color0");
        NoteColorOverride[1] = noteData->_color1;
    }
}

void NoteColorizer::DisableNoteColorOverride() {
    NoteColorOverride[0] = std::nullopt;
    NoteColorOverride[1] = std::nullopt;
}

void NoteColorizer::ColorizeSaber(GlobalNamespace::NoteController *noteController,
                                  GlobalNamespace::NoteCutInfo *noteCutInfo) {
    // TODO: Actually implement this
    if (ChromaController::DoColorizerSabers)
        {
        NoteData* noteData = noteController->noteData;
        SaberType saberType = noteCutInfo->saberType;
        if ((int)noteData->colorType == (int)saberType)
        {
            UnityEngine::Color color = CNVColorManager::GetCNVColorManager(noteController)->ColorForCNVManager();

            SaberColorizer::SetSaberColor(saberType, color);
        }
    }
}

void NoteColorizer::CNVStart(GlobalNamespace::ColorNoteVisuals *cnv, GlobalNamespace::NoteController *nc) {
    ColorType noteType = nc->noteData->colorType;
    if (noteType == ColorType::ColorA || noteType == ColorType::ColorB)
    {
        CNVColorManager::CreateCNVColorManager(cnv, nc);
    }
}

NoteColorizer::CNVColorManager::CNVColorManager(GlobalNamespace::ColorNoteVisuals *cnv,
                                                GlobalNamespace::NoteController *nc) {
    _cnv = cnv;
    _nc = nc;
    _colorManager = cnv->colorManager;
    if (il2cpp_functions::class_is_assignable_from(nc->noteData->klass, classof(CustomJSONData::CustomNoteData*))) {
        _noteData = reinterpret_cast<CustomJSONData::CustomNoteData *>(nc->noteData);
    }
}

NoteColorizer::CNVColorManager *
NoteColorizer::CNVColorManager::GetCNVColorManager(GlobalNamespace::NoteController *nc) {

    for (auto& n : _cnvColorManagers) {
        if (n->_nc == nc)
            return n;
    }

    return nullptr;
}

NoteColorizer::CNVColorManager *
NoteColorizer::CNVColorManager::CreateCNVColorManager(GlobalNamespace::ColorNoteVisuals *cnv,
                                                      GlobalNamespace::NoteController *nc) {
    CNVColorManager *cnvColorManager = GetCNVColorManager(nc);
    if (cnvColorManager != nullptr)
    {
        if (il2cpp_functions::class_is_assignable_from(nc->noteData->klass, classof(CustomNoteData*))) {
            auto *customData = reinterpret_cast<CustomNoteData *>(nc->noteData);
            cnvColorManager->_noteData = customData;
            customData->customData->associatedData['C'] = new CustomData::NoteData {_globalColor[0], _globalColor[1]};
        }

        return nullptr;
    }

    auto* cnvcm = new CNVColorManager(cnv, nc);
    _cnvColorManagers.push_back(cnvcm);
    return cnvcm;
}

void NoteColorizer::CNVColorManager::SetGlobalNoteColors(std::optional<UnityEngine::Color> color0,
                                                         std::optional<UnityEngine::Color> color1) {
    if (color0)
    {
        _globalColor[0] = color0.value();
    }

    if (color1)
    {
        _globalColor[1] = color1.value();
    }
}

void NoteColorizer::CNVColorManager::ResetGlobal() {
    _globalColor[0] = std::nullopt;
    _globalColor[1] = std::nullopt;
}

void NoteColorizer::CNVColorManager::Reset() {
    _noteData->customData->associatedData['C'] = new CustomData::NoteData {_globalColor[0], _globalColor[1]};
}

void NoteColorizer::CNVColorManager::SetNoteColors(std::optional<UnityEngine::Color> color0,
                                                   std::optional<UnityEngine::Color> color1) {
    if (color0 || color1) {
        CustomData::NoteData *noteData;
        CustomData::NoteData *oldNoteData = static_cast<CustomData::NoteData *>(_noteData->customData->associatedData['C']);
        memcpy(&noteData, &oldNoteData, sizeof oldNoteData);

        if (color0) {
            noteData->_color0 = color0.value();
        }

        if (color1) {
            noteData->_color1 = color1.value();
        }

        _noteData->customData->associatedData['C'] = noteData;
    }
}

UnityEngine::Color NoteColorizer::CNVColorManager::ColorForCNVManager() {
    EnableNoteColorOverride(_nc);
    UnityEngine::Color noteColor = _colorManager->ColorForType(_noteData->colorType);
    DisableNoteColorOverride();
    return noteColor;
}

UnityEngine::Color ColorWithAlpha(UnityEngine::Color color, float a) {
    color.a = a;
    return color;
}

void NoteColorizer::CNVColorManager::SetActiveColors() {
    ColorNoteVisuals *colorNoteVisuals = _cnv;

    UnityEngine::Color noteColor = ColorForCNVManager();

    SpriteRenderer* arrowGlowSpriteRenderer = colorNoteVisuals->arrowGlowSpriteRenderer;
    SpriteRenderer* circleGlowSpriteRenderer = colorNoteVisuals->circleGlowSpriteRenderer;
    arrowGlowSpriteRenderer->set_color(ColorWithAlpha(noteColor, arrowGlowSpriteRenderer->get_color().a));
    circleGlowSpriteRenderer->set_color(ColorWithAlpha(noteColor, circleGlowSpriteRenderer->get_color().a));
    Array<MaterialPropertyBlockController *> *materialPropertyBlockControllers = colorNoteVisuals->materialPropertyBlockControllers;
    for (int i = 0; i < materialPropertyBlockControllers->Length(); i++)
    {
        if (_colorID == -1) {
            _colorID = UnityEngine::Shader::PropertyToID(il2cpp_utils::createcsstr("_Color"));
        }

        MaterialPropertyBlockController *materialPropertyBlockController = materialPropertyBlockControllers->values[i];
        materialPropertyBlockController->materialPropertyBlock->SetColor(_colorID, noteColor);
        materialPropertyBlockController->ApplyChanges();
    }
}


