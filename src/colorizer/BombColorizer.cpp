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

std::optional<UnityEngine::Color> BombColorizer::BNCColorManager::_globalColor = std::nullopt;

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
    ResetAllBombColors();
    _bncColorManagers.clear();
}

void BombColorizer::BNCStart(GlobalNamespace::BombNoteController *bnc) {
    BNCColorManager::CreateBNCColorManager(bnc);
}

BombColorizer::BNCColorManager::BNCColorManager(GlobalNamespace::BombNoteController *nc) {
    _nc = nc;

    _bombMaterial = nc->noteTransform->get_gameObject()->GetComponent<Renderer*>()->get_material();

    _color_Original = _bombMaterial->GetColor(il2cpp_utils::createcsstr("_SimpleColor"));
    if (_globalColor)
    {
        _bombMaterial->SetColor(il2cpp_utils::createcsstr("_SimpleColor"), _globalColor.value());
    }
}

BombColorizer::BNCColorManager *BombColorizer::BNCColorManager::GetBNCColorManager(GlobalNamespace::BombNoteController *nc) {
    for (auto& n : _bncColorManagers) {
        if (n->_nc == nc)
            return n;
    }

    return nullptr;
}

BombColorizer::BNCColorManager *
BombColorizer::BNCColorManager::CreateBNCColorManager(GlobalNamespace::BombNoteController *nc) {
    if (GetBNCColorManager(nc) != nullptr)
    {
        return nullptr;
    }

    auto* bnccm = new BNCColorManager(nc);
    _bncColorManagers.push_back(bnccm);
    return bnccm;
}

void BombColorizer::BNCColorManager::SetGlobalBombColor(std::optional<UnityEngine::Color> color) {
    if (color)
    {
        _globalColor = color.value();
    }
}

void BombColorizer::BNCColorManager::ResetGlobal() {
    _globalColor = std::nullopt;
}

void BombColorizer::BNCColorManager::Reset() {
    if (_globalColor)
    {
        _bombMaterial->SetColor(il2cpp_utils::createcsstr("_SimpleColor"), _globalColor.value());
    }
    else
    {
        _bombMaterial->SetColor(il2cpp_utils::createcsstr("_SimpleColor"), _color_Original);
    }
}

void BombColorizer::BNCColorManager::SetBombColor(std::optional<UnityEngine::Color> color) {
    if (color)
    {
        _bombMaterial->SetColor(il2cpp_utils::createcsstr("_SimpleColor"), color.value());
    }
}