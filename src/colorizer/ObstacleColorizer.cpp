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

DEFINE_TYPE(Chroma::OCColorManager);

void ObstacleColorizer::Reset(GlobalNamespace::ObstacleController *oc) {
    auto ocman = OCColorManager::GetOCColorManager(oc);

    if (ocman)
        ocman->Reset();
}

void ObstacleColorizer::ResetAllObstacleColors() {
    OCColorManager::ResetGlobal();

    for (auto& ocColorManager : _ocColorManagers)
    {
        ocColorManager.second->Reset();
    }
}

void
ObstacleColorizer::SetObstacleColor(GlobalNamespace::ObstacleController *oc, UnityEngine::Color color) {
    auto ocman = OCColorManager::GetOCColorManager(oc);

    if (ocman)
        ocman->SetObstacleColor(color);
}

void ObstacleColorizer::SetAllObstacleColors(UnityEngine::Color color) {
    OCColorManager::SetGlobalObstacleColor(color);

    for (auto& ocColorManager : _ocColorManagers)
    {
        ocColorManager.second->Reset();
    }
}

void ObstacleColorizer::SetActiveColors(GlobalNamespace::ObstacleController *oc) {
    OCColorManager::GetOCColorManager(oc)->SetActiveColors();
}

void ObstacleColorizer::SetAllActiveColors() {
    for (auto& ocColorManager : _ocColorManagers)
    {
        ocColorManager.second->SetActiveColors();
    }
}

void ObstacleColorizer::ClearOCColorManagers() {
    // TODO: Should we do this?
    //    ResetAllObstacleColors();
    _ocColorManagers.clear();
}

void ObstacleColorizer::OCStart(GlobalNamespace::ObstacleController *oc, UnityEngine::Color original) {
    OCColorManager::CreateOCColorManager(oc, original);
}



void OCColorManager::ctor(GlobalNamespace::ObstacleController *oc, UnityEngine::Color original) {
    _oc = oc;
    _stretchableObstacle = _oc->stretchableObstacle;

    _color_Original = original;

    _color = _color_Original;
}

OCColorManager *
OCColorManager::GetOCColorManager(GlobalNamespace::ObstacleController *oc) {
    if (!oc)
        return nullptr;

    auto it = ObstacleColorizer::_ocColorManagers.find(oc);

    if (it != ObstacleColorizer::_ocColorManagers.end()) return it->second;

    return nullptr;
}

OCColorManager *
OCColorManager::CreateOCColorManager(GlobalNamespace::ObstacleController *oc, UnityEngine::Color original) {
    if (GetOCColorManager(oc) != nullptr)
    {
        return nullptr;
    }

    OCColorManager* occm = CRASH_UNLESS(il2cpp_utils::New<OCColorManager*>(oc, original));
    ObstacleColorizer::_ocColorManagers[oc] = occm;
    return occm;
}

void OCColorManager::SetGlobalObstacleColor(UnityEngine::Color color) {
    _globalColor = color;
}

void OCColorManager::ResetGlobal() {
    _globalColor = std::nullopt;
}

void OCColorManager::Reset() {
    _color = std::nullopt;
}

void OCColorManager::SetObstacleColor(UnityEngine::Color color) {

    _color = color;

}

void OCColorManager::SetActiveColors() const {
    Color finalColor = _color.value_or(_globalColor.value_or(_color_Original));

    ParametricBoxFrameController* obstacleFrame = _stretchableObstacle->obstacleFrame;

    if (finalColor == obstacleFrame->color) return;


    ParametricBoxFakeGlowController* obstacleFakeGlow = _stretchableObstacle->obstacleFakeGlow;
    Array<MaterialPropertyBlockController *> *materialPropertyBlockControllers = _stretchableObstacle->materialPropertyBlockControllers;

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
            _tintColorID = UnityEngine::Shader::PropertyToID(il2cpp_utils::newcsstr("_TintColor"));
        }

        if (_addColorID == -1) {
            _addColorID = UnityEngine::Shader::PropertyToID(il2cpp_utils::newcsstr("_AddColor"));
        }

        MaterialPropertyBlockController* materialPropertyBlockController = materialPropertyBlockControllers->get(i);
        materialPropertyBlockController->materialPropertyBlock->SetColor(_addColorID, value);
        materialPropertyBlockController->materialPropertyBlock->SetColor(_tintColorID, UnityEngine::Color::Lerp(finalColor, UnityEngine::Color::get_white(), obstacleCoreLerpToWhiteFactor));
        materialPropertyBlockController->ApplyChanges();
    }
}
