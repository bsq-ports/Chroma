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

std::optional<UnityEngine::Color> ObstacleColorizer::OCColorManager::_globalColor = std::nullopt;



void ObstacleColorizer::Reset(GlobalNamespace::ObstacleController *oc) {
    auto ocman = OCColorManager::GetOCColorManager(oc);

    if (ocman)
        ocman->Reset();
}

void ObstacleColorizer::ResetAllObstacleColors() {
    OCColorManager::ResetGlobal();

    for (auto ocColorManager : _ocColorManagers)
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

    for (auto ocColorManager : _ocColorManagers)
    {
        ocColorManager->Reset();
    }
}

void ObstacleColorizer::SetActiveColors(GlobalNamespace::ObstacleController *oc) {
    OCColorManager::GetOCColorManager(oc)->SetActiveColors();
}

void ObstacleColorizer::SetAllActiveColors() {
    for (auto ocColorManager : _ocColorManagers)
    {
        ocColorManager->SetActiveColors();
    }
}

void ObstacleColorizer::ClearOCColorManagers() {
    ResetAllObstacleColors();
    _ocColorManagers.clear();
}

void ObstacleColorizer::OCStart(GlobalNamespace::ObstacleController *oc) {
    OCColorManager::CreateOCColorManager(oc);
}

ObstacleColorizer::OCColorManager::OCColorManager(GlobalNamespace::ObstacleController *oc) {
    _oc = oc;
    _stretchableObstacle = _oc->stretchableObstacle;

    _color_Original = oc->color->color;

    if (_color == nullptr)
    {
        // TODO: We might need custom type here
        _color = ScriptableObject::CreateInstance<SimpleColorSO*>();
        _color->SetColor(_color_Original);
    }

    oc->color = _color;
}

ObstacleColorizer::OCColorManager *
ObstacleColorizer::OCColorManager::GetOCColorManager(GlobalNamespace::ObstacleController *oc) {
    for (auto n : _ocColorManagers) {
        if (n->_oc == oc)
            return n;
    }

    return nullptr;
}

ObstacleColorizer::OCColorManager *
ObstacleColorizer::OCColorManager::CreateOCColorManager(GlobalNamespace::ObstacleController *oc) {
    if (GetOCColorManager(oc) != nullptr)
    {
        return nullptr;
    }

    auto occm = new OCColorManager(oc);
    _ocColorManagers.push_back(occm);
    return occm;
}

void ObstacleColorizer::OCColorManager::SetGlobalObstacleColor(std::optional<UnityEngine::Color> color) {
    if (color)
    {
        _globalColor = color.value();
    }
}

void ObstacleColorizer::OCColorManager::ResetGlobal() {
    _globalColor = std::nullopt;
}

void ObstacleColorizer::OCColorManager::Reset() {
    if (_globalColor)
    {
        _color->SetColor(_globalColor.value());
    }
    else
    {
        _color->SetColor(_color_Original);
    }
}

void ObstacleColorizer::OCColorManager::SetObstacleColor(std::optional<UnityEngine::Color> color) {
    if (color)
    {
        _color->SetColor(color.value());
    }
}

void ObstacleColorizer::OCColorManager::SetActiveColors() {
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
