#include "Chroma.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/SpriteRenderer.hpp"
#include "UnityEngine/MaterialPropertyBlock.hpp"

#include <unordered_map>
#include "colorizer/BombColorizer.hpp"
#include "utils/ChromaUtils.hpp"

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Sombrero;
using namespace Chroma;


BombColorizer::ColorizerMap BombColorizer::Colorizers = BombColorizer::ColorizerMap();


void BombColorizer::Refresh() {
    UnityEngine::Material* bombMaterial = _bombRenderer->get_material();
    Sombrero::FastColor const& color = getColor();

    static auto colorName = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("_SimpleColor");

    if (color == bombMaterial->GetColor(colorName)) {
        return;
    }

    bombMaterial->SetColor(colorName, color);
}

BombColorizer::BombColorizer(GlobalNamespace::NoteControllerBase *noteController) :
    _bombRenderer(noteController->get_gameObject()->GetComponentInChildren<Renderer*>())
{
    static auto colorName = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("_SimpleColor");

    OriginalColor = _bombRenderer->get_material()->GetColor(colorName);
}

std::optional<Sombrero::FastColor> BombColorizer::getGlobalColor() {
    return GlobalColor;
}

void BombColorizer::GlobalColorize(std::optional<Sombrero::FastColor> const& color) {
    GlobalColor = color;
    for (auto& valuePair : Colorizers)
    {
        valuePair.second->Refresh();
    }
}

std::shared_ptr<BombColorizer> BombColorizer::New(GlobalNamespace::NoteControllerBase *noteController) {
    std::shared_ptr<BombColorizer> bombColorizer(new BombColorizer(noteController));

    Colorizers[noteController] = bombColorizer;

    return bombColorizer;
}

std::optional<Sombrero::FastColor> BombColorizer::GlobalColorGetter() {
    return GlobalColor;
}

void BombColorizer::Reset() {
    GlobalColor = std::nullopt;
    Colorizers.clear();
    Colorizers = {};
}




