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
#include "UnityEngine/LineRenderer.hpp"
#include "UnityEngine/ParticleSystem.hpp"
#include "UnityEngine/ParticleSystem_MainModule.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/ParticleSystem_MinMaxGradient.hpp"
#include "hooks/SaberManager.hpp"

#include <unordered_map>
#include "utils/ChromaUtils.hpp"
#include "colorizer/SaberColorizer.hpp"

using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;
using namespace custom_types::Helpers;
using namespace Chroma;

std::vector<std::optional<UnityEngine::Color>> SaberColorizer::SaberColorOverride = std::vector<std::optional<UnityEngine::Color>>(2, std::nullopt);

void SaberColorizer::SetSaberColor(int saberType, std::optional<UnityEngine::Color> color) {
    for (auto& bms : SaberColorizer::BSMColorManager::GetBSMColorManager(saberType)) {
        getLogger().debug("Coloring a saber");
        bms->SetSaberColor(color);
    }
}

void SaberColorizer::SetAllSaberColors(std::optional<UnityEngine::Color> color0, std::optional<UnityEngine::Color> color1) {
    for (auto& bms : SaberColorizer::BSMColorManager::GetBSMColorManager(SaberType::SaberA)) {
        bms->SetSaberColor(color0);
    }

    for (auto& bms : SaberColorizer::BSMColorManager::GetBSMColorManager(SaberType::SaberB)) {
        bms->SetSaberColor(color1);
    }
}

void SaberColorizer::ClearBSMColorManagers() {
    _bsmColorManagers.clear();
}

void SaberColorizer::BSMStart(GlobalNamespace::Saber *bcm, int saberType) {
    if (saberType == SaberType::SaberA || saberType == SaberType::SaberB)
    {
        getLogger().debug("Saber start was called, doing stuff");
        BSMColorManager::CreateBSMColorManager(bcm, saberType);
    }
}

SaberColorizer::BSMColorManager::BSMColorManager(GlobalNamespace::Saber *bsm, int saberType) {
    _saberType = saberType;
}

std::vector<SaberColorizer::BSMColorManager *>SaberColorizer::BSMColorManager::GetBSMColorManager(int saberType) {
    std::vector<BSMColorManager *> saberColors;

    getLogger().debug("Checking all the beat saber models %d", _bsmColorManagers.size());
    for (auto& man : _bsmColorManagers) {
        getLogger().debug("Is %d also %d", man->_saberType, saberType);
        if (man->_saberType == saberType)
            saberColors.push_back(man);
    }

    return saberColors;
}

SaberColorizer::BSMColorManager* SaberColorizer::BSMColorManager::CreateBSMColorManager(GlobalNamespace::Saber *bsm,
                                                                                         int saberType) {
    getLogger().debug("Creating a beat saber model manager");
    auto* bsmcm = new BSMColorManager(bsm, saberType);
    _bsmColorManagers.push_back(bsmcm);
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
    getLogger().debug("Coloring saber model %d", (int) instance->get_saberType());

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
    getLogger().debug("Model controller klass %s", modelController->klass->name);

    TubeBloomPrePassLight *saberLight = modelController->saberLight;


    if (saberLight) {
        saberLight->color = color;
    } else {
        getLogger().debug("Saber light is null, should be normal right?");
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
custom_types::Helpers::Coroutine ChangeColorCoroutine(Saber *saber, UnityEngine::Color color) {
    co_yield reinterpret_cast<enumeratorT *>(CRASH_UNLESS(
            il2cpp_utils::New<UnityEngine::WaitForSecondsRealtime *>(0.05f)));


    getLogger().debug("Change color coroutine");
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

    auto saberBurnMarkSparkles = saber->GetComponentInChildren<SaberBurnMarkSparkles *>();

    if (saberBurnMarkSparkles) {
        auto saberMarkPS = saberBurnMarkSparkles->burnMarksPS;

        for (int i = 0; i < saberMarkPS->Length(); i++) {
            auto sPS = saberMarkPS->values[i];

            sPS->get_main().set_startColor(ParticleSystem::MinMaxGradient(saberColor));
        }
    }

    auto saberBurnMarkArea = saber->GetComponentInChildren<SaberBurnMarkArea *>();
    if (saberBurnMarkArea) {

        auto lineRenderers = saberBurnMarkArea->lineRenderers;

        for (int i = 0; i < lineRenderers->Length(); i++) {
            auto lineRenderer = lineRenderers->values[i];

            lineRenderer->set_startColor(saberColor);
            lineRenderer->set_endColor(saberColor);
            lineRenderer->set_positionCount(2);
        }
    }


    coroutineSabers.erase(saber->get_saberType().value);

    co_return;
}

// Must be down here to avoid compile issues
void SaberColorizer::BSMColorManager::SetSaberColor(std::optional<UnityEngine::Color> colorNullable) {
    if (colorNullable)
    {
        auto _bsm = getSaber();
        auto runningCoro = coroutineSabers.find(_saberType);
        if (runningCoro != coroutineSabers.end()) {
            _bsm->StopCoroutine(reinterpret_cast<enumeratorT *>(runningCoro->second));
            coroutineSabers.erase(runningCoro);
        }

        custom_types::Helpers::StandardCoroutine* coro = custom_types::Helpers::CoroutineHelper::New(ChangeColorCoroutine(_bsm, colorNullable.value()));

        _bsm->StartCoroutine(reinterpret_cast<enumeratorT*>(coro));
        coroutineSabers[_saberType] = coro;
    }
}

GlobalNamespace::Saber *SaberColorizer::BSMColorManager::getSaber() const {
    return Chroma::SaberManagerHolder::saberManager->SaberForType(_saberType);
}
