#include "Chroma.hpp"

#include "UnityEngine/SpriteRenderer.hpp"
#include "UnityEngine/MaterialPropertyBlock.hpp"
#include "GlobalNamespace/MaterialPropertyBlockController.hpp"

#include <unordered_map>
#include "colorizer/NoteColorizer.hpp"
#include "colorizer/SaberColorizer.hpp"
#include "ChromaController.hpp"
#include "utils/ChromaUtils.hpp"


using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;

using namespace Chroma;
//
//std::vector<std::optional<UnityEngine::Color>> NoteColorizer::SaberColorOverride = std::vector<std::optional<UnityEngine::Color>>(2);
//
//std::vector<SaberColorizer::BSMColorManager *> SaberColorizer::_bsmColorManagers;

std::vector<std::optional<UnityEngine::Color>> NoteColorizer::NoteColorOverride = std::vector<std::optional<UnityEngine::Color>>(2, std::nullopt);
std::vector<std::optional<UnityEngine::Color>> NoteColorizer::CNVColorManager::_globalColor = std::vector<std::optional<UnityEngine::Color>>(2, std::nullopt);

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

void NoteColorizer::SetNoteColors(GlobalNamespace::NoteController *cnv, std::optional<UnityEngine::Color> color0, std::optional<UnityEngine::Color> color1) {
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
    if (il2cpp_functions::class_is_assignable_from(noteController->noteData->klass, classof(CustomNoteData*))) {
        auto *customData = reinterpret_cast<CustomNoteData *>(noteController->noteData);

        if (customData->customData && customData->customData->value) {
            auto &dynData = *customData->customData->value;


            // TODO: Do these execute a similar or exact implementation of the PC version at
            // https://github.com/Aeroluna/Chroma/blob/e7a72f8b848c822d860361a027034218125af135/Chroma/Colorizer/NoteColorizer.cs#L71-L72


            if (dynData.MemberCount() > 0) {
//                PrintJSONValue(dynData);
                NoteColorOverride[0] = ChromaUtils::ChromaUtilities::GetColorFromData(&dynData, "color0");
            } else {
                NoteColorOverride[0] = std::nullopt;
            }

            if (dynData.MemberCount() > 1) {
                NoteColorOverride[1] = ChromaUtils::ChromaUtilities::GetColorFromData(&dynData, "color1");
            } else {
                NoteColorOverride[1] = std::nullopt;
            }


        }
    }
}

void NoteColorizer::DisableNoteColorOverride() {
    NoteColorOverride[0] = std::nullopt;
    NoteColorOverride[1] = std::nullopt;
}

bool MatchesColorType(SaberType saberType, ColorType colorType){
    static_assert(sizeof(ColorType) == sizeof(int));
    getLogger().debug("%i saber %i saberA %i colorType %i colorTypeA", saberType.value, SaberType::_get_SaberA().value, colorType.value, ColorType::_get_ColorA().value);
    return (saberType.value == SaberType::SaberA && colorType.value == ColorType::ColorA)
    || (saberType.value == SaberType::SaberB && colorType.value == ColorType::ColorB);
}


void NoteColorizer::ColorizeSaber(GlobalNamespace::NoteController *noteController, GlobalNamespace::NoteCutInfo *noteCutInfo) {
    // TODO: Actually implement this
    getLogger().debug("Coloring sabers");
    if (ChromaController::DoColorizerSabers())
        {
        NoteData* noteData = noteController->noteData;
        auto saberType = noteCutInfo->saberType;
        auto colorType = noteData->colorType;
        getLogger().debug("Oh wait I'm actually coloring them %d vs saber %d", colorType.value, saberType.value);
        if (MatchesColorType(saberType, colorType))
        {
            getLogger().debug("Ok now time to color");
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

std::optional<UnityEngine::Color> NoteColorizer::getNoteColorOverride(int color) {
    if (NoteColorizer::NoteColorOverride.size() < color + 1)
        return std::nullopt;

    return NoteColorizer::NoteColorOverride[color];
}

NoteColorizer::CNVColorManager::CNVColorManager(GlobalNamespace::ColorNoteVisuals *cnv, GlobalNamespace::NoteController *nc) {
    _cnv = cnv;
    _nc = nc;
    _colorManager = cnv->colorManager;
    if (il2cpp_functions::class_is_assignable_from(nc->noteData->klass, classof(CustomJSONData::CustomNoteData*))) {
        _noteData = reinterpret_cast<CustomJSONData::CustomNoteData *>(nc->noteData);
        _noteData->customData->associatedData['C'] = new CustomData::NoteData {_globalColor[0], _globalColor[1]};
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

//            if (customData->customData && customData->customData->value) {
//                cnvColorManager->_noteData = customData;
//                customData->customData->value->GetObject()["color0"] = _globalColor[0];
//                customData->customData->value->GetObject()["color1"] = _globalColor[0];
//            }
        }

        return nullptr;
    }

    auto* cnvcm = new CNVColorManager(cnv, nc);
    _cnvColorManagers.push_back(cnvcm);
    return cnvcm;
}

void NoteColorizer::CNVColorManager::SetGlobalNoteColors(std::optional<UnityEngine::Color> color0,std::optional<UnityEngine::Color> color1) {
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

void NoteColorizer::CNVColorManager::SetNoteColors(std::optional<UnityEngine::Color> color0, std::optional<UnityEngine::Color> color1) {
    if (color0 || color1) {
        CustomData::NoteData *noteData = static_cast<CustomData::NoteData *>(_noteData->customData->associatedData['C']);

        if (color0) {
            noteData->_color0 = color0.value();
        }

        if (color1) {
            noteData->_color1 = color1.value();
        }

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


