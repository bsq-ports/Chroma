#include "Chroma.hpp"

#include "UnityEngine/MaterialPropertyBlock.hpp"
#include "GlobalNamespace/MaterialPropertyBlockController.hpp"
#include "GlobalNamespace/ParametricBoxFrameController.hpp"
#include "GlobalNamespace/ParametricBoxFakeGlowController.hpp"
#include "UnityEngine/Shader.hpp"

#include <unordered_map>
#include "colorizer/ObstacleColorizer.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"


using namespace GlobalNamespace;
using namespace UnityEngine;

using namespace Chroma;

DEFINE_CLASS(Chroma::OCColorManager);

void ObstacleColorizer::Reset(GlobalNamespace::ObstacleController *oc) {
    auto ocman = OCColorManager::GetOCColorManager(oc);

    if (ocman)
        ocman->Reset();
}

void ObstacleColorizer::ResetAllObstacleColors() {
    OCColorManager::ResetGlobal();

    for (auto& ocColorManager : _ocColorManagers)
    {
        ocColorManager->Reset();
    }
}

void
ObstacleColorizer::SetObstacleColor(GlobalNamespace::ObstacleController *oc, std::optional<UnityEngine::Color> color) {
    auto ocman = OCColorManager::GetOCColorManager(oc);

    if (ocman)
        ocman->SetObstacleColor(color);
}

void ObstacleColorizer::SetAllObstacleColors(std::optional<UnityEngine::Color> color) {
    OCColorManager::SetGlobalObstacleColor(color);

    for (auto& ocColorManager : _ocColorManagers)
    {
        ocColorManager->Reset();
    }
}

void ObstacleColorizer::SetActiveColors(GlobalNamespace::ObstacleController *oc) {
    OCColorManager::GetOCColorManager(oc)->SetActiveColors();
}

void ObstacleColorizer::SetAllActiveColors() {
    for (auto& ocColorManager : _ocColorManagers)
    {
        ocColorManager->SetActiveColors();
    }
}

void ObstacleColorizer::ClearOCColorManagers() {
    // TODO: Should we do this?
    //    ResetAllObstacleColors();
    _ocColorManagers.clear();
}

void ObstacleColorizer::OCStart(GlobalNamespace::ObstacleController *oc) {
    OCColorManager::CreateOCColorManager(oc);
}



void OCColorManager::ctor(GlobalNamespace::ObstacleController *oc) {
    _oc = oc;
    _stretchableObstacle = _oc->stretchableObstacle;

    _color_Original = oc->color->color;

    if (_color == nullptr)
    {
        _color = ScriptableObject::CreateInstance<SimpleColorSO*>();
        _color->SetColor(_color_Original);
    }

    oc->color = _color;
}

OCColorManager *
OCColorManager::GetOCColorManager(GlobalNamespace::ObstacleController *oc) {
    if (!oc)
        return nullptr;

    for (auto& n : ObstacleColorizer::_ocColorManagers) {
        if (n && n->_oc == oc)
            return n;
    }

    return nullptr;
}

OCColorManager *
OCColorManager::CreateOCColorManager(GlobalNamespace::ObstacleController *oc) {
    if (GetOCColorManager(oc) != nullptr)
    {
        return nullptr;
    }

    OCColorManager* occm = CRASH_UNLESS(il2cpp_utils::New<OCColorManager*>(oc));
    ObstacleColorizer::_ocColorManagers.push_back(occm);
    return occm;
}

void OCColorManager::SetGlobalObstacleColor(std::optional<UnityEngine::Color> color) {
    if (color)
    {
        _globalColor = color.value();
    }
}

void OCColorManager::ResetGlobal() {
    _globalColor = std::nullopt;
}

void OCColorManager::Reset() {
    if (!_color) return;

    if (_globalColor)
    {
        _color->SetColor(_globalColor.value());
    }
    else
    {
        _color->SetColor(_color_Original);
    }
}

void OCColorManager::SetObstacleColor(std::optional<UnityEngine::Color> color) const {
    if (color)
    {
        _color->SetColor(color.value());
    }
}

void OCColorManager::SetActiveColors() const {
    ParametricBoxFrameController* obstacleFrame = _stretchableObstacle->obstacleFrame;
    ParametricBoxFakeGlowController* obstacleFakeGlow = _stretchableObstacle->obstacleFakeGlow;
    Array<MaterialPropertyBlockController *> *materialPropertyBlockControllers = _stretchableObstacle->materialPropertyBlockControllers;
    UnityEngine::Color finalColor = _color->color;
    obstacleFrame->color = finalColor;
    obstacleFrame->Refresh();
    obstacleFakeGlow->color = finalColor;
    obstacleFakeGlow->Refresh();
    UnityEngine::Color value = finalColor * _stretchableObstacle->addColorMultiplier;
    value.a = 0.0f;
    float obstacleCoreLerpToWhiteFactor = _stretchableObstacle->obstacleCoreLerpToWhiteFactor;
    for (int i = 0; i < materialPropertyBlockControllers->Length(); i++)
    {
        if (_tintColorID == -1) {
            _tintColorID = UnityEngine::Shader::PropertyToID(il2cpp_utils::createcsstr("_TintColor"));
        }

        if (_addColorID == -1) {
            _addColorID = UnityEngine::Shader::PropertyToID(il2cpp_utils::createcsstr("_AddColor"));
        }

        MaterialPropertyBlockController* materialPropertyBlockController = materialPropertyBlockControllers->values[i];
        materialPropertyBlockController->materialPropertyBlock->SetColor(_addColorID, value);
        materialPropertyBlockController->materialPropertyBlock->SetColor(_tintColorID, UnityEngine::Color::Lerp(finalColor, UnityEngine::Color::get_white(), obstacleCoreLerpToWhiteFactor));
        materialPropertyBlockController->ApplyChanges();
    }
}
