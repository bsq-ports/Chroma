#include "ui/ModifierViewController.hpp"

#include "ChromaConfig.hpp"
#include "UnityEngine/RectOffset.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Sprite.hpp"

#include "UnityEngine/Transform.hpp"
#include "bsml/shared/BSML.hpp"
#include "main.hpp"
#include "assets.hpp"

using namespace Chroma;
using namespace UnityEngine::UI;
using namespace UnityEngine;

DEFINE_TYPE(Chroma, ModifierViewController)

void Chroma::ModifierViewController::DidActivate(bool firstActivation) {
  if (firstActivation) {
    BSML::parse_and_construct(IncludedAssets::ui_bsml, transform, this);
  }
}

void Chroma::ModifierViewController::DidActivate2(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
  if (firstActivation) {
    BSML::parse_and_construct(IncludedAssets::ui_bsml, transform, this);
  }
}

bool Chroma::ModifierViewController::get_environmentEnhancementsEnabled() {
    return getChromaConfig().environmentEnhancementsEnabled.GetValue();
}
void Chroma::ModifierViewController::set_environmentEnhancementsEnabled(bool value) {
    getChromaConfig().environmentEnhancementsEnabled.SetValue(value, true);
}

bool Chroma::ModifierViewController::get_customNoteColors() {
    return getChromaConfig().customNoteColors.GetValue();
}
void Chroma::ModifierViewController::set_customNoteColors(bool value) {
    getChromaConfig().customNoteColors.SetValue(value, true);
}

bool Chroma::ModifierViewController::get_customColorEventsEnabled() {
    return getChromaConfig().customColorEventsEnabled.GetValue();
}
void Chroma::ModifierViewController::set_customColorEventsEnabled(bool value) {
    getChromaConfig().customColorEventsEnabled.SetValue(value, true);
}