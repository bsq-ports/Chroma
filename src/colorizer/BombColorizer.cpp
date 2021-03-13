#include "Chroma.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/SpriteRenderer.hpp"
#include "UnityEngine/MaterialPropertyBlock.hpp"

#include <unordered_map>
#include "colorizer/BombColorizer.hpp"

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

using namespace GlobalNamespace;
using namespace UnityEngine;

using namespace Chroma;

DEFINE_CLASS(Chroma::BNCColorManager);

std::optional<UnityEngine::Color> BNCColorManager::_globalColor = std::nullopt;

void BombColorizer::Reset(GlobalNamespace::BombNoteController *bnc) {
    auto bncman = BNCColorManager::GetBNCColorManager(bnc);

    if (bncman)
        bncman->Reset();
}

void BombColorizer::ResetAllBombColors() {
    BNCColorManager::ResetGlobal();

    for (auto& bncColorManager : _bncColorManagers)
    {
        bncColorManager->Reset();
    }
}

void BombColorizer::SetBombColor(GlobalNamespace::BombNoteController *bnc, std::optional<UnityEngine::Color> color) {
    auto bncman = BNCColorManager::GetBNCColorManager(bnc);

    if (bncman)
        bncman->SetBombColor(color);
}

void BombColorizer::SetAllBombColors(std::optional<UnityEngine::Color> color) {
    BNCColorManager::SetGlobalBombColor(color);

    for (auto& bncColorManager : _bncColorManagers)
    {
        bncColorManager->Reset();
    }
}

void BombColorizer::ClearBNCColorManagers() {
    // TODO: Should we even do this?
    //    ResetAllBombColors();
    _bncColorManagers.clear();
}

void BombColorizer::BNCStart(GlobalNamespace::BombNoteController *bnc) {
    BNCColorManager::CreateBNCColorManager(bnc);
}

void BNCColorManager::ctor(GlobalNamespace::BombNoteController *nc) {
    _nc = nc;

    _bombMaterial = nc->noteTransform->get_gameObject()->GetComponent<Renderer*>()->get_material();

    _color_Original = _bombMaterial->GetColor(il2cpp_utils::createcsstr("_SimpleColor"));
    if (_globalColor)
    {
        _bombMaterial->SetColor(il2cpp_utils::createcsstr("_SimpleColor"), _globalColor.value());
    }
}

BNCColorManager *BNCColorManager::GetBNCColorManager(GlobalNamespace::BombNoteController *nc) {
    for (auto& n : BombColorizer::_bncColorManagers) {
        if (n->_nc == nc)
            return n;
    }

    return nullptr;
}

BNCColorManager *
BNCColorManager::CreateBNCColorManager(GlobalNamespace::BombNoteController *nc) {
    if (GetBNCColorManager(nc) != nullptr)
    {
        return nullptr;
    }

    auto* bnccm = CRASH_UNLESS(il2cpp_utils::New<BNCColorManager*>(nc));
    BombColorizer::_bncColorManagers.push_back(bnccm);
    return bnccm;
}

void BNCColorManager::SetGlobalBombColor(std::optional<UnityEngine::Color> color) {
    if (color)
    {
        _globalColor = color.value();
    }
}

void BNCColorManager::ResetGlobal() {
    _globalColor = std::nullopt;
}

void BNCColorManager::Reset() {
    if (_bombMaterial) {
        if (_globalColor) {
            _bombMaterial->SetColor(il2cpp_utils::createcsstr("_SimpleColor"), _globalColor.value());
        } else {
            _bombMaterial->SetColor(il2cpp_utils::createcsstr("_SimpleColor"), _color_Original);
        }
    }
}

void BNCColorManager::SetBombColor(std::optional<UnityEngine::Color> color) const {
    if (color)
    {
        _bombMaterial->SetColor(il2cpp_utils::createcsstr("_SimpleColor"), color.value());
    }
}
