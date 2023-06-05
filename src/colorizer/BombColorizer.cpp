#include "Chroma.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/SpriteRenderer.hpp"
#include "UnityEngine/Shader.hpp"
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
    Sombrero::FastColor const& color = getColor();

    if (!_materialPropertyBlockController) {
        _materialPropertyBlockController = CRASH_UNLESS(noteController->GetComponent<GlobalNamespace::MaterialPropertyBlockController*>());
    }
    // getter here causes the null value to instantiate
    // which then allows later use through field
    auto bombMaterial = _materialPropertyBlockController->get_materialPropertyBlock();
    if (color == static_cast<Sombrero::FastColor>(bombMaterial->GetColor(_simpleColor()))) {
        return;
    }

    BombColorChanged.invoke(noteController, color);

    if (BombColorable) return;

    bombMaterial->SetColor(_simpleColor(), color);
    bombMaterial->SetColor(_color(), color);
    _materialPropertyBlockController->ApplyChanges();
}

BombColorizer::BombColorizer(GlobalNamespace::NoteControllerBase *noteController) :
        noteController(noteController),
        _materialPropertyBlockController(noteController->GetComponent<GlobalNamespace::MaterialPropertyBlockController*>())
{
    OriginalColor = noteController->GetComponentInChildren<Renderer*>()->get_material()->GetColor(_simpleColor());
    // getter here causes the null value to instantiate
    // which then allows later use through field
//    auto materialPropertyBlock = _materialPropertyBlockController->get_materialPropertyBlock();
//    materialPropertyBlock->SetColor(_simpleColor(), OriginalColor);
//    materialPropertyBlock->SetColor(_color(), OriginalColor);
}

std::optional<Sombrero::FastColor> BombColorizer::getGlobalColor() {
    return GlobalColor;
}

void BombColorizer::GlobalColorize(std::optional<Sombrero::FastColor> const& color) {
    GlobalColor = color;
    for (auto& [_, colorizer] : Colorizers)
    {
        colorizer.Refresh();
    }
}

BombColorizer& BombColorizer::New(GlobalNamespace::NoteControllerBase *noteController) {
    return Colorizers.try_emplace(noteController, noteController).first->second;
}

std::optional<Sombrero::FastColor> BombColorizer::GlobalColorGetter() {
    return GlobalColor;
}

void BombColorizer::Reset() {
    GlobalColor = std::nullopt;
    Colorizers.clear();
    BombColorChanged.clear();
}

int BombColorizer::_simpleColor() {
    static int colorID = UnityEngine::Shader::PropertyToID("_SimpleColor");

    return colorID;
}

int BombColorizer::_color() {
    static int colorID = UnityEngine::Shader::PropertyToID("_Color");

    return colorID;
}




