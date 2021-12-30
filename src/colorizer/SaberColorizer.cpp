#include "Chroma.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/WaitForSecondsRealtime.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/MaterialPropertyBlock.hpp"
#include "GlobalNamespace/SaberModelController.hpp"
#include "GlobalNamespace/SetSaberGlowColor.hpp"
#include "GlobalNamespace/SetSaberFakeGlowColor.hpp"
#include "GlobalNamespace/TubeBloomPrePassLight.hpp"
#include "GlobalNamespace/SaberModelController_InitData.hpp"
#include "GlobalNamespace/SaberTrail.hpp"
#include "GlobalNamespace/SetSaberGlowColor_PropertyTintColorPair.hpp"
#include "GlobalNamespace/Parametric3SliceSpriteController.hpp"
#include "GlobalNamespace/SaberManager.hpp"
#include "GlobalNamespace/SaberModelController.hpp"
#include "GlobalNamespace/SaberBurnMarkSparkles.hpp"
#include "GlobalNamespace/ColorManager.hpp"
#include "UnityEngine/LineRenderer.hpp"
#include "UnityEngine/ParticleSystem.hpp"
#include "UnityEngine/ParticleSystem_MainModule.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/ParticleSystem_MinMaxGradient.hpp"
#include "UnityEngine/Resources.hpp"
#include "hooks/SaberManager.hpp"

#include <unordered_map>
#include "utils/ChromaUtils.hpp"
#include "colorizer/SaberColorizer.hpp"

#include <cstdlib>

using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;
using namespace custom_types::Helpers;
using namespace Chroma;
using namespace Sombrero;

SaberColorizer::SaberColorizer(GlobalNamespace::Saber *saber) {
    _saberType = saber->get_saberType();

    _saberModelController = saber->get_gameObject()->GetComponentInChildren<SaberModelController *>(true);

    _saberTrail = _saberModelController->saberTrail;
    _trailTintColor = _saberModelController->initData->trailTintColor;
    _saberLight = _saberModelController->saberLight;


    _lastColor = _saberModelController->colorManager->ColorForSaberType(_saberType);
    OriginalColor = _lastColor;

}

std::shared_ptr<SaberColorizer> SaberColorizer::New(GlobalNamespace::Saber *saber) {
    std::shared_ptr<SaberColorizer> saberColorizer(new SaberColorizer(saber));

    Colorizers.emplace(saberColorizer->_saberModelController, saberColorizer);

    return saberColorizer;
}

std::optional<Sombrero::FastColor> SaberColorizer::GlobalColorGetter() {
    return GlobalColor[(int) _saberType];
}

void SaberColorizer::GlobalColorize(GlobalNamespace::SaberType saberType, std::optional<Sombrero::FastColor> const& color) {
    GlobalColor[(int) saberType] = color;
    for (auto &c : GetColorizerList(saberType)) {
        c->Refresh();
    }
}

void SaberColorizer::Reset() {
    GlobalColor[0] = std::nullopt;
    GlobalColor[1] = std::nullopt;
    Colorizers.clear();
    Colorizers = {};

    SaberColorChanged.clear();
    ColorableModels.clear();
}

void SaberColorizer::Refresh() {
    Sombrero::FastColor const& color = getColor();
    if (color == Sombrero::FastColor(_lastColor))
    {
        return;
    }


    static auto SetColor = FPtrWrapper<static_cast<void (UnityEngine::MaterialPropertyBlock::*)(Il2CppString*, UnityEngine::Color)>(&UnityEngine::MaterialPropertyBlock::SetColor)>::get();
    static auto SetPropertyBlock = FPtrWrapper<static_cast<void (UnityEngine::Renderer::*)(UnityEngine::MaterialPropertyBlock*)>(&UnityEngine::Renderer::SetPropertyBlock)>::get();
    static auto Refresh = FPtrWrapper<&Parametric3SliceSpriteController::Refresh>::get();

    _lastColor = color;
    if (!IsColorable(_saberModelController))
    {
        auto _setSaberGlowColors = _saberModelController->setSaberGlowColors;
        auto _setSaberFakeGlowColors = _saberModelController->setSaberFakeGlowColors;

        auto saberTrail = _saberTrail;
        saberTrail->color = (color * _trailTintColor).Linear();

        if (_setSaberGlowColors) {
            for (auto setSaberGlowColor : _setSaberGlowColors) {
                if (!setSaberGlowColor)
                    continue;

                MaterialPropertyBlock* materialPropertyBlock = setSaberGlowColor->materialPropertyBlock;
                if (!materialPropertyBlock) {
                    setSaberGlowColor->materialPropertyBlock = MaterialPropertyBlock::New_ctor();
                    materialPropertyBlock = setSaberGlowColor->materialPropertyBlock;
                }

                auto propertyTintColorPairs = setSaberGlowColor->propertyTintColorPairs;

                if (propertyTintColorPairs) {
                    for (auto &propertyTintColorPair : propertyTintColorPairs) {
                        if (propertyTintColorPair)
                            SetColor(materialPropertyBlock, propertyTintColorPair->property, color * Sombrero::FastColor(propertyTintColorPair->tintColor));
                    }
                }

                if (setSaberGlowColor->meshRenderer)
                    SetPropertyBlock(setSaberGlowColor->meshRenderer, materialPropertyBlock);
            }
        }

        if (_setSaberFakeGlowColors) {
            for (auto setSaberFakeGlowColor : _setSaberFakeGlowColors) {
                if (!setSaberFakeGlowColor) continue;

                auto parametric3SliceSprite = setSaberFakeGlowColor->parametric3SliceSprite;
                parametric3SliceSprite->color = color * setSaberFakeGlowColor->tintColor;
                Refresh(parametric3SliceSprite);
            }
        }

        if (_saberLight)
        {
            _saberLight->color = color;
        }
    }
    else
    {
        ColorColorable(color);
    }

    SaberColorChanged.invoke(_saberType, _saberModelController, color);
}

std::shared_ptr<SaberColorizer> &
SaberColorizer::GetColorizer(GlobalNamespace::SaberModelController *saberModelController) {
    return Colorizers[saberModelController];
}

void SaberColorizer::RemoveColorizer(GlobalNamespace::SaberModelController *saberModelController) {
    Colorizers.erase(saberModelController);
}

std::unordered_set<SaberColorizer*> SaberColorizer::GetColorizerList(GlobalNamespace::SaberType saberType) {
    std::unordered_set<SaberColorizer*> colorizers;

    for (auto const& saber : Colorizers) {
        if (saber.second->_saberType.value == saberType.value) {
            colorizers.emplace(saber.second.get());
        }
    }

    return colorizers;
}

void SaberColorizer::ColorColorable(Sombrero::FastColor color) {
    // Nothing here I guess
}

void SaberColorizer::SetColorable(GlobalNamespace::SaberModelController *saberModelController, bool colorable) {
    if (colorable)
        ColorableModels.emplace(saberModelController);
    else
        ColorableModels.erase(saberModelController);
}

bool SaberColorizer::IsColorable(GlobalNamespace::SaberModelController *saberModelController) {
    return ColorableModels.contains(saberModelController);
}
