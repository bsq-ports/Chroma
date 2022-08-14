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
    if (!_bombRenderer->get_enabled()) return;

    Sombrero::FastColor const& color = getColor();

    UnityEngine::Material* bombMaterial = _bombRenderer->get_material();
    if (color == static_cast<Sombrero::FastColor>(bombMaterial->GetColor(_simpleColor()))) {
        return;
    }

    BombColorChanged.invoke(noteController, color);

    if (BombColorable) return;

    bombMaterial->SetColor(_simpleColor(), color);
}

BombColorizer::BombColorizer(GlobalNamespace::NoteControllerBase *noteController) :
    _bombRenderer(noteController->get_gameObject()->GetComponentInChildren<Renderer*>()),
    noteController(noteController)
{
    OriginalColor = _bombRenderer->get_material()->GetColor(_simpleColor());
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




