#include "Chroma.hpp"

#include "UnityEngine/SpriteRenderer.hpp"
#include "UnityEngine/MaterialPropertyBlock.hpp"
#include "UnityEngine/MeshRenderer.hpp"

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
//
//std::vector<std::optional<UnityEngine::Color>> NoteColorizer::SaberColorOverride = std::vector<std::optional<UnityEngine::Color>>(2);
//
//std::vector<SaberColorizer::BSMColorManager *> SaberColorizer::_bsmColorManagers;

std::unordered_map<int ,std::optional<UnityEngine::Color>> NoteColorizer::NoteColorOverride = {{0, std::nullopt}, {1, std::nullopt}};
std::unordered_map<int ,std::optional<UnityEngine::Color>> NoteColorizer::CNVColorManager::GlobalColor = {{0, std::nullopt}, {1, std::nullopt}};

void NoteColorizer::Reset(GlobalNamespace::NoteController *nc) {
    auto m = CNVColorManager::GetCNVColorManager(nc);

    if (m)
        m->Reset();
}

void NoteColorizer::ResetAllNotesColors() {
    CNVColorManager::ResetGlobal();

    for (const auto& cnvColorManager : _cnvColorManagers)
    {
        cnvColorManager.second->Reset();
    }
}

void NoteColorizer::SetNoteColors(GlobalNamespace::NoteController *cnv, std::optional<UnityEngine::Color> color0, std::optional<UnityEngine::Color> color1) {
    auto m = CNVColorManager::GetCNVColorManager(cnv);

    if (m)
        m->SetNoteColors(color0, color1);
}

void
NoteColorizer::SetAllNoteColors(std::optional<UnityEngine::Color> color0, std::optional<UnityEngine::Color> color1) {
    CNVColorManager::SetGlobalNoteColors(color0, color1);

    for (const auto& cnvColorManager : _cnvColorManagers)
    {
        cnvColorManager.second->Reset();
    }
}

void NoteColorizer::SetActiveColors(GlobalNamespace::NoteController *nc) {
    CNVColorManager::GetCNVColorManager(nc)->SetActiveColors();
}

void NoteColorizer::SetAllActiveColors() {
    for (const auto& cnvColorManager : _cnvColorManagers)
    {
        cnvColorManager.second->SetActiveColors();
    }
}

void NoteColorizer::ClearCNVColorManagers() {
    ResetAllNotesColors();
    _cnvColorManagers.clear();
}



//std::optional<UnityEngine::Color> getColor(rapidjson::Value& json) {
//    auto jsonArray = json.GetArray();
//
//    if (jsonArray.Size() < 3)
//        return std::nullopt;
//
//
//    return UnityEngine::Color(jsonArray[0].GetFloat(), jsonArray[1].GetFloat(), jsonArray[2].GetFloat(), jsonArray.Size() > 3 ? jsonArray[4].GetFloat() : 1.0f);
//}

void NoteColorizer::EnableNoteColorOverride(GlobalNamespace::NoteController *noteController) {
    auto chromaData = ChromaObjectDataManager::ChromaObjectDatas[noteController->noteData];

    if (chromaData->Color) {
        NoteColorOverride[0] = chromaData->Color;
        NoteColorOverride[1] = chromaData->Color;
    } else {
        NoteColorOverride[0] = CNVColorManager::GlobalColor[0];
        NoteColorOverride[1] = CNVColorManager::GlobalColor[1];
    }
}

void NoteColorizer::DisableNoteColorOverride() {
    NoteColorOverride[0] = std::nullopt;
    NoteColorOverride[1] = std::nullopt;
}


void NoteColorizer::ColorizeSaber(GlobalNamespace::NoteController *noteController, GlobalNamespace::NoteCutInfo *noteCutInfo) {
    if (ChromaController::DoColorizerSabers())
        {
        NoteData* noteData = noteController->noteData;
        auto saberType = noteCutInfo->saberType;
        auto colorType = noteData->colorType;
        if ((int)colorType == (int) saberType)
        {
            UnityEngine::Color color = CNVColorManager::GetCNVColorManager(noteController)->ColorForCNVManager();

            SaberColorizer::SetSaberColor(saberType, color);
        }
    }
}

void NoteColorizer::CNVStart(GlobalNamespace::ColorNoteVisuals *cnv, GlobalNamespace::NoteController *nc) {
    ColorType noteType = nc->noteData->colorType;
    if ((int) noteType == (int) ColorType::ColorA || (int) noteType == (int) ColorType::ColorB)
    {
        CNVColorManager::CreateCNVColorManager(cnv, nc);
    }
}

std::optional<UnityEngine::Color> NoteColorizer::getNoteColorOverride(int color) {
    return NoteColorizer::NoteColorOverride[color];
}

NoteColorizer::CNVColorManager::CNVColorManager(GlobalNamespace::ColorNoteVisuals *cnv, GlobalNamespace::NoteController *nc) {
    _cnv = cnv;
    _nc = nc;
    _colorManager = cnv->colorManager;

    _chromaData = std::static_pointer_cast<ChromaNoteData>(ChromaObjectDataManager::ChromaObjectDatas[nc->noteData]);

    auto castedNote = il2cpp_utils::try_cast<CustomJSONData::CustomNoteData>(nc->noteData);

    if (castedNote) {
        _noteData =  *castedNote;
    }
}

std::shared_ptr<NoteColorizer::CNVColorManager> NoteColorizer::CNVColorManager::GetCNVColorManager(GlobalNamespace::NoteController *nc) {
    auto it = _cnvColorManagers.find(nc);

    if (it != _cnvColorManagers.end()) return it->second;

    return nullptr;
}

std::shared_ptr<NoteColorizer::CNVColorManager> NoteColorizer::CNVColorManager::CreateCNVColorManager(GlobalNamespace::ColorNoteVisuals *cnv,
                                                      GlobalNamespace::NoteController *nc) {
    std::shared_ptr<CNVColorManager> cnvColorManager = GetCNVColorManager(nc);
    if (cnvColorManager != nullptr)
    {

        // Theoretically, this should never be called
        //        CRASH_UNLESS(false);
        // However it does and I can't wonder why,
        // TODO: gotta fix Aero's NoteColorizer for them
        auto castedNote = il2cpp_utils::try_cast<CustomJSONData::CustomNoteData>(nc->noteData);
        if (castedNote) {
            auto *customData = *castedNote;
            cnvColorManager->_noteData = customData;

            cnvColorManager->_chromaData = std::static_pointer_cast<ChromaNoteData>(
                    ChromaObjectDataManager::ChromaObjectDatas[nc->noteData]);
            // Aero why
            //           cnvColorManager->Reset();

        }

        return nullptr;
    }

    std::shared_ptr<CNVColorManager> cnvcm = std::make_shared<CNVColorManager>(cnv, nc);
    _cnvColorManagers[nc] = cnvcm;
    return cnvcm;
}

void NoteColorizer::CNVColorManager::SetGlobalNoteColors(std::optional<UnityEngine::Color> color0,std::optional<UnityEngine::Color> color1) {
    if (color0)
    {
        GlobalColor[0] = color0.value();
    }

    if (color1)
    {
        GlobalColor[1] = color1.value();
    }
}

void NoteColorizer::CNVColorManager::ResetGlobal() {
    GlobalColor[0] = std::nullopt;
    GlobalColor[1] = std::nullopt;
}

void NoteColorizer::CNVColorManager::Reset() {
    _chromaData->Color = std::nullopt;
}

void NoteColorizer::CNVColorManager::SetNoteColors(std::optional<UnityEngine::Color> color0, std::optional<UnityEngine::Color> color1) {
    if (_noteData->colorType == (int) GlobalNamespace::ColorType::ColorA) {
        _chromaData->Color = color0;
    } else if (_noteData->colorType == (int) GlobalNamespace::ColorType::ColorB) {
        _chromaData->Color = color1;
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

    if (noteColor == colorNoteVisuals->noteColor) {
        return;
    }

    colorNoteVisuals->noteColor = noteColor;
    Array<MaterialPropertyBlockController *> *materialPropertyBlockControllers = colorNoteVisuals->materialPropertyBlockControllers;
    for (int i = 0; i < materialPropertyBlockControllers->Length(); i++)
    {
        auto *materialPropertyBlockController = materialPropertyBlockControllers->get(i);
        materialPropertyBlockController->materialPropertyBlock->SetColor(_colorID, ColorWithAlpha(noteColor, 1.0f));
        materialPropertyBlockController->ApplyChanges();
    }

    if (colorNoteVisuals->didInitEvent) {
        colorNoteVisuals->didInitEvent->Invoke(colorNoteVisuals, colorNoteVisuals->noteController);
    }
}


