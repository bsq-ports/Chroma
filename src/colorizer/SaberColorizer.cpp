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

#include <unordered_map>
#include "colorizer/SaberColorizer.hpp"



using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;

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
    _bsm = bsm;
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

DEFINE_CLASS(Chroma::ChangeColorCoroutine);

void Chroma::ChangeColorCoroutine::ctor(Saber *instance, UnityEngine::Color color) {
    this->instance = instance;
    this->color = color;
    this->current = nullptr;
    this->hasWaited = false;
}

void Chroma::ChangeColorCoroutine::Reset() {

}

Il2CppObject* Chroma::ChangeColorCoroutine::get_Current() {
    return current;
}

// https://github.com/Auros/SiraUtil/blob/f40d4ca44f5e6632ed74606cb4d6676546f6f56e/SiraUtil/Sabers/SiraSaber.cs#L164
bool Chroma::ChangeColorCoroutine::MoveNext() {
    getLogger().debug("Waiting for change color coroutine");
    if (!hasWaited) {
        current = WaitForSecondsRealtime::New_ctor(0.05f);
        hasWaited = true;
        return true; // Continue coroutine
    }

    getLogger().debug("Change color coroutine");
    if (instance->get_isActiveAndEnabled()) {
        // In th epc version, this starts another coroutine which does the color changing
        // However, I believe we can get hopefully away without making another coroutine class since
        // the wait time is set to 0 anyways

        getLogger().debug("Coloring saber model");

        auto *modelController = instance->get_gameObject()->GetComponentInChildren<SaberModelController *>(true);


//        if (modelController is IColorable colorable)
//        {
//            colorable.SetColor(color);
//        }

        auto tintColor = modelController->initData->trailTintColor;
        Array<SetSaberGlowColor *> *setSaberGlowColors = modelController->setSaberGlowColors;
        Array<SetSaberFakeGlowColor *> *setSaberFakeGlowColors = modelController->setSaberFakeGlowColors;
        TubeBloomPrePassLight *light = modelController->saberLight;

        modelController->saberTrail->color = (color * tintColor).get_linear();

        for (int i = 0; i != setSaberGlowColors->Length(); i++)
        {
            OverrideColor(setSaberGlowColors->values[i], color);
        }
        for (int i = 0; i < setSaberFakeGlowColors->Length(); i++)
        {
            OverrideColor(setSaberFakeGlowColors->values[i], color);
        }
        if (light != nullptr)
        {
            light->color = color;
        }

    }
    coroutineSabers[instance] = nullptr;
    coroutineSabers.erase(instance);

    current = nullptr;

    return false; // Reached end of coroutine
}

IEnumerator *ChangeColorCo(Saber *instance, UnityEngine::Color color) {
    Chroma::ChangeColorCoroutine *coroutine = CRASH_UNLESS(il2cpp_utils::New<Chroma::ChangeColorCoroutine*>(instance, color));
    return reinterpret_cast<IEnumerator*>(coroutine);
}

// Must be down here to avoid compile issues
void SaberColorizer::BSMColorManager::SetSaberColor(std::optional<UnityEngine::Color> colorNullable) {
    if (colorNullable)
    {
        if (coroutineSabers.contains(_bsm)) {
            _bsm->StopCoroutine(reinterpret_cast<IEnumerator*>(coroutineSabers[_bsm]));
            coroutineSabers[_bsm] = nullptr;
            coroutineSabers.erase(_bsm);
        }

        coroutineSabers[_bsm] = ChangeColorCo(_bsm, colorNullable.value());
        _bsm->StartCoroutine(coroutineSabers[_bsm]);
//        getLogger().debug("Start the change color coroutine");
//        _bsm->StartCoroutine(ChangeColorCo(_bsm, colorNullable.value()));
    }
}