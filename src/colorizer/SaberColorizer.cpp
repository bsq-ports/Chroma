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

#include <dlfcn.h>
#include <stdlib.h>

using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;
using namespace custom_types::Helpers;
using namespace Chroma;



// Sira utils methods
void OverrideColor(SetSaberGlowColor* ssgc, UnityEngine::Color color) {
    MeshRenderer* mesh = ssgc->meshRenderer;
    MaterialPropertyBlock* block = ssgc->materialPropertyBlock;
    Array<SetSaberGlowColor::PropertyTintColorPair *> *tintPairs = ssgc->propertyTintColorPairs;
    if (block == nullptr)
    {
        block = MaterialPropertyBlock::New_ctor();
    }
    for (int i = 0; i < tintPairs->Length(); i++)
    {
        SetSaberGlowColor::PropertyTintColorPair *ptcp = tintPairs->get(i);
        block->SetColor(ptcp->property, color * ptcp->tintColor);
    }
    mesh->SetPropertyBlock(block);
}

void OverrideColor(SetSaberFakeGlowColor* ssfgc, UnityEngine::Color color) {
    Parametric3SliceSpriteController* sliceSpriteController = ssfgc->parametric3SliceSprite;
    sliceSpriteController->color = color * ssfgc->tintColor;
    sliceSpriteController->Refresh();
}

void ChangeColorCoroutine2(Saber *instance, UnityEngine::Color color) {
    auto *modelController = instance->get_gameObject()->GetComponentInChildren<SaberModelController *>(true);


//        if (modelController is IColorable colorable)
//        {
//            colorable.SetColor(color);
//        }

    auto tintColor = modelController->initData->trailTintColor;
    Array<SetSaberGlowColor *> *setSaberGlowColors = modelController->setSaberGlowColors;
    Array<SetSaberFakeGlowColor *> *setSaberFakeGlowColors = modelController->setSaberFakeGlowColors;

    modelController->saberTrail->color = (color * tintColor).get_linear();

    for (int i = 0; i != setSaberGlowColors->Length(); i++) {
        OverrideColor(setSaberGlowColors->get(i), color);
    }
    for (int i = 0; i < setSaberFakeGlowColors->Length(); i++) {
        OverrideColor(setSaberFakeGlowColors->get(i), color);
    }

    TubeBloomPrePassLight *saberLight = modelController->saberLight;


    if (saberLight) {
        saberLight->color = color;
    }
}

/// These color functions convert color to a single int so we can use it in extern functions
/// Since extern functions can't return UnityEngine::Color
float linearToGamma(float c) {
    return 255.0f * std::pow(c / 255.0f, 1 / 2.2f);
}

UnityEngine::Color getGamma(UnityEngine::Color color) {
    return UnityEngine::Color(linearToGamma(color.r), linearToGamma(color.g), linearToGamma(color.b), color.a);
}

UnityEngine::Color getSaberColor(Saber* saber) {
    auto* modelController = saber->get_gameObject()->GetComponentInChildren<SaberModelController*>(true);
//    if (modelController is IColorable)
//    {
//        var colorable = modelController as IColorable;
//        return colorable.Color;
//    }

    return getGamma(modelController->saberTrail->color); // smc.Accessors.TrailColor(ref Accessors.SaberTrail(ref smc)).gamma;

}

// https://github.com/Auros/SiraUtil/blob/f40d4ca44f5e6632ed74606cb4d6676546f6f56e/SiraUtil/Sabers/SiraSaber.cs#L164
custom_types::Helpers::Coroutine ChangeColorCoroutine(Saber *saber, UnityEngine::Color color, SaberBurnMarkSparkles* saberBurnMarkSparkles, SaberBurnMarkArea* saberBurnMarkArea) {
    co_yield reinterpret_cast<enumeratorT *>(CRASH_UNLESS(
            il2cpp_utils::New<UnityEngine::WaitForSecondsRealtime *>(0.05f)));


    if (saber->get_isActiveAndEnabled()) {
        // In the pc version, this starts another coroutine which does the color changing
        // However, I believe we can get hopefully away without making another coroutine class since
        // the wait time is set to 0 anyways

        ChangeColorCoroutine2(saber, color);
    }


    UnityEngine::Color saberColor = getSaberColor(saber);
    float h;
    float s;
    float ignored;
    UnityEngine::Color::RGBToHSV(saberColor, h, s, ignored);
    saberColor = UnityEngine::Color::HSVToRGB(h, s, 1.0f);



    if (saberBurnMarkSparkles) {
        auto saberMarkPS = saberBurnMarkSparkles->burnMarksPS;

        auto sPS = saberMarkPS->get((int) saber->get_saberType());

        sPS->get_main().set_startColor(ParticleSystem::MinMaxGradient(saberColor));
    }


    if (saberBurnMarkArea) {

        auto lineRenderers = saberBurnMarkArea->lineRenderers;

        // TODO: Find out if this breaks saber burn marks
        if (lineRenderers) {
            auto lineRenderer = lineRenderers->get((int) saber->get_saberType());

            if (lineRenderer) {
                lineRenderer->set_startColor(saberColor);
                lineRenderer->set_endColor(saberColor);
                lineRenderer->set_positionCount(2);
            }
        }
    }

    // Call callbacks
    SaberColorizer::SaberColorChanged.invoke(saber->get_saberType(), saberColor);


    coroutineSabers.erase(saber->get_saberType().value);

    co_return;
}
// SIRA UTIL METHODS
SaberColorizer::SaberColorizer(GlobalNamespace::Saber *saber) {
    _saberType = saber->get_saberType();

    _saberModelController = saber->get_gameObject()->GetComponentInChildren<SaberModelController *>(true);

    _doColor = true;

    _saberTrail = _saberModelController->saberTrail;
    _trailTintColor = _saberModelController->initData->trailTintColor;
    _saberLight = _saberModelController->saberLight;


    _lastColor = _saberModelController->colorManager->ColorForSaberType(_saberType);
    OriginalColor = _lastColor;
}

std::shared_ptr<SaberColorizer> SaberColorizer::New(GlobalNamespace::Saber *saber) {
    std::shared_ptr<SaberColorizer> saberColorizer(new SaberColorizer(saber));
    GetOrCreateColorizerList(saberColorizer->_saberType).emplace(saberColorizer);

    return saberColorizer;
}

std::optional<UnityEngine::Color> SaberColorizer::GlobalColorGetter() {
    return GlobalColor[(int) _saberType];
}

void SaberColorizer::GlobalColorize(GlobalNamespace::SaberType saberType, std::optional<UnityEngine::Color> color) {
    GlobalColor[(int)saberType] = color;
    for (auto& c : GetSaberColorizer(saberType))
        c->Refresh();
}

void SaberColorizer::Reset() {
    GlobalColor[0] = std::nullopt;
    GlobalColor[1] = std::nullopt;
    Colorizers.clear();
    Colorizers = {};
    SaberColorChanged.clear();
}

void SaberColorizer::Refresh() {
    Color color = getColor();
    if (ChromaUtils::ColorEquals(color, _lastColor))
    {
        return;
    }

    _lastColor = color;
    if (_doColor)
    {
        Array<GlobalNamespace::SetSaberGlowColor*>* _setSaberGlowColors = _saberModelController->setSaberGlowColors;
        Array<GlobalNamespace::SetSaberFakeGlowColor*>* _setSaberFakeGlowColors = _saberModelController->setSaberFakeGlowColors;

        auto saberTrail = _saberTrail;
        saberTrail->color = (color * _trailTintColor).get_linear();

        if (_setSaberGlowColors) {
            for (int i = 0; i < _setSaberGlowColors->Length(); i++) {
                auto setSaberGlowColor = _setSaberGlowColors->get(i);

                if (!setSaberGlowColor)
                    continue;

                MaterialPropertyBlock* materialPropertyBlock = setSaberGlowColor->materialPropertyBlock;
                if (!materialPropertyBlock) {
                    setSaberGlowColor->materialPropertyBlock = MaterialPropertyBlock::New_ctor();
                    materialPropertyBlock = setSaberGlowColor->materialPropertyBlock;
                }

                auto propertyTintColorPairs = setSaberGlowColor->propertyTintColorPairs;

                if (propertyTintColorPairs && propertyTintColorPairs->Length() > 0) {
                    std::vector<SetSaberGlowColor::PropertyTintColorPair *> propertyTintColorPairsVec;
                    propertyTintColorPairs->copy_to(propertyTintColorPairsVec);
                    for (auto &propertyTintColorPair : propertyTintColorPairsVec) {
                        if (propertyTintColorPair)
                            materialPropertyBlock->SetColor(propertyTintColorPair->property,
                                                            color * propertyTintColorPair->tintColor);
                    }
                }

                if (setSaberGlowColor->meshRenderer)
                    setSaberGlowColor->meshRenderer->SetPropertyBlock((MaterialPropertyBlock *) materialPropertyBlock);
            }
        }

        if (_setSaberFakeGlowColors) {
            for (int i = 0; i < _setSaberFakeGlowColors->Length(); i++) {
                auto setSaberFakeGlowColor = _setSaberFakeGlowColors->get(i);
                if (!setSaberFakeGlowColor) continue;

                auto parametric3SliceSprite = setSaberFakeGlowColor->parametric3SliceSprite;
                parametric3SliceSprite->color = color * setSaberFakeGlowColor->tintColor;
                parametric3SliceSprite->Refresh();
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
    float h;
    float s;
    float _;

    Color::RGBToHSV(color, h, s, _);
    Color effectColor = Color::HSVToRGB(h, s, 1);
    SaberColorChanged.invoke(_saberType, effectColor);
}

std::unordered_set<std::shared_ptr<SaberColorizer>>&
SaberColorizer::GetOrCreateColorizerList(GlobalNamespace::SaberType saberType) {
    auto it = Colorizers.find(saberType.value);

    if (it == Colorizers.end()) {
        Colorizers[saberType.value] = std::unordered_set<std::shared_ptr<SaberColorizer>>();
        return Colorizers[saberType.value];
    }

    return it->second;
}

bool SaberColorizer::IsColorable(GlobalNamespace::SaberModelController *saberModelController) {
    return false;
}

void SaberColorizer::ColorColorable(UnityEngine::Color color) {

}

