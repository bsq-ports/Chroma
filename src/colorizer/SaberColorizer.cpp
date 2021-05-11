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

std::vector<std::optional<UnityEngine::Color>> SaberColorizer::SaberColorOverride = {std::nullopt, std::nullopt};

void SaberColorizer::SetSaberColor(int saberType, UnityEngine::Color color) {
    auto scm = SaberColorizer::BSMColorManager::GetBSMColorManager(saberType);

    if (scm)
        scm->SetSaberColor(color);
}

void SaberColorizer::SetAllSaberColors(std::optional<UnityEngine::Color> color0, std::optional<UnityEngine::Color> color1) {

    if (color0) {
        auto scm = SaberColorizer::BSMColorManager::GetBSMColorManager(SaberType::SaberA);

        if (scm)
            scm->SetSaberColor(color0.value());
    }

    if (color1) {
        auto scm = SaberColorizer::BSMColorManager::GetBSMColorManager(SaberType::SaberB);

        if (scm)
            scm->SetSaberColor(color1.value());
    }
}

void SaberColorizer::ClearBSMColorManagers() {
    SaberColorOverride = {std::nullopt, std::nullopt};
    _bsmColorManagers.clear();
    clearCallbacks();
}

void SaberColorizer::BSMStart(GlobalNamespace::Saber *bcm, int saberType) {
    if (saberType == SaberType::SaberA || saberType == SaberType::SaberB)
    {
        getLogger().debug("Saber start was called, doing stuff");
        BSMColorManager::CreateBSMColorManager(bcm, saberType);
    }
}

void SaberColorizer::registerCallback(std::function<void()> callback) {
    saberCallbacks.push_back(callback);
}

void SaberColorizer::clearCallbacks() {
    saberCallbacks.clear();
}

SaberColorizer::BSMColorManager::BSMColorManager(GlobalNamespace::Saber *bsm, int saberType) {
    _saberType = saberType;
    saberBurnMarkArea = UnityEngine::Resources::FindObjectsOfTypeAll<SaberBurnMarkArea*>()->values[0];
    saberBurnMarkSparkles = UnityEngine::Resources::FindObjectsOfTypeAll<SaberBurnMarkSparkles*>()->values[0];
}

std::shared_ptr<SaberColorizer::BSMColorManager> SaberColorizer::BSMColorManager::GetBSMColorManager(int saberType) {
    auto it = _bsmColorManagers.find(saberType);

    if (it != _bsmColorManagers.end()) return it->second;

    return nullptr;
}

std::shared_ptr<SaberColorizer::BSMColorManager> SaberColorizer::BSMColorManager::CreateBSMColorManager(GlobalNamespace::Saber *bsm,
                                                                                         int saberType) {
    getLogger().debug("Creating a beat saber model manager");
    std::shared_ptr<BSMColorManager> bsmcm = std::make_shared<SaberColorizer::BSMColorManager>(bsm, saberType);
    _bsmColorManagers[saberType] = bsmcm;
    return bsmcm;
}


// Sira utils methods
void OverrideColor(SetSaberGlowColor* ssgc, UnityEngine::Color color) {
    MeshRenderer* mesh = ssgc->meshRenderer;
    MaterialPropertyBlock* block = ssgc->materialPropertyBlock;
    Array<SetSaberGlowColor::PropertyTintColorPair *> *tintPairs = ssgc->propertyTintColorPairs;
    if (block == nullptr)
    {
        block = new MaterialPropertyBlock();
    }
    for (int i = 0; i < tintPairs->Length(); i++)
    {
        SetSaberGlowColor::PropertyTintColorPair *ptcp = tintPairs->values[i];
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
        OverrideColor(setSaberGlowColors->values[i], color);
    }
    for (int i = 0; i < setSaberFakeGlowColors->Length(); i++) {
        OverrideColor(setSaberFakeGlowColors->values[i], color);
    }

    TubeBloomPrePassLight *saberLight = modelController->saberLight;


    if (saberLight) {
        saberLight->color = color;
    }
}

float linearToGamma(float c) {
    return 255.0f * std::pow(c/255.0f, 1/2.2f);
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

        auto sPS = saberMarkPS->values[(int) saber->get_saberType()];

        sPS->get_main().set_startColor(ParticleSystem::MinMaxGradient(saberColor));
    }


    if (saberBurnMarkArea) {

        auto lineRenderers = saberBurnMarkArea->lineRenderers;

        // TODO: Find out if this breaks saber burn marks
        if (lineRenderers) {
            auto lineRenderer = lineRenderers->values[(int) saber->get_saberType()];

            if (lineRenderer) {
                lineRenderer->set_startColor(saberColor);
                lineRenderer->set_endColor(saberColor);
                lineRenderer->set_positionCount(2);
            }
        }
    }

    // Call callbacks
    for (auto& c : SaberColorizer::saberCallbacks)
    {
        c();
    }


    coroutineSabers.erase(saber->get_saberType().value);

    co_return;
}

// Must be down here to avoid compile issues
void SaberColorizer::BSMColorManager::SetSaberColor(UnityEngine::Color color) {
    if (color == _lastColor) {
        return;
    }

    _lastColor = color;
    SaberColorOverride[_saberType] = color;
    auto _bsm = getSaber();
    auto runningCoro = coroutineSabers.find(_saberType);
    if (runningCoro != coroutineSabers.end()) {
        _bsm->StopCoroutine(reinterpret_cast<enumeratorT *>(runningCoro->second));
        coroutineSabers.erase(runningCoro);
    }

    custom_types::Helpers::StandardCoroutine *coro = custom_types::Helpers::CoroutineHelper::New(
            ChangeColorCoroutine(_bsm, color, saberBurnMarkSparkles, saberBurnMarkArea)
    );

    _bsm->StartCoroutine(reinterpret_cast<enumeratorT *>(coro));
    coroutineSabers[_saberType] = coro;

}

GlobalNamespace::Saber *SaberColorizer::BSMColorManager::getSaber() const {
    return Chroma::SaberManagerHolder::saberManager->SaberForType(_saberType);
}
