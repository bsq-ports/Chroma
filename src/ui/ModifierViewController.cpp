#include "ui/ModifierViewController.hpp"

#include "UnityEngine/RectOffset.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Sprite.hpp"

#include "main.hpp"

using namespace Chroma;
using namespace UnityEngine::UI;
using namespace UnityEngine;

DEFINE_TYPE(Chroma, ModifierViewController)

UnityEngine::Sprite* UIUtils::configToIcon(ConfigUtils::ConfigValue<bool> const& configValue) {
  static const std::unordered_map<ConfigUtils::ConfigValue<bool> const*, std::string> spriteNameMap = {
    { &getChromaConfig().environmentEnhancementsEnabled, "GlobalIcon" },
    { &getChromaConfig().customColorEventsEnabled, "LightIcon" },
    { &getChromaConfig().customNoteColors, "DisappearingArrows" }
  };
  static std::unordered_map<std::string_view, UnityEngine::Sprite*> spriteMap;

  auto const& name = spriteNameMap.at(&configValue);

  auto& sprite = spriteMap[name];

  if (UnityW(sprite) == nullptr) {
    sprite = Resources::FindObjectsOfTypeAll<Sprite*>()->First(
                                       [&name](Sprite* x) { return x->get_name() == name; });
  }

  return sprite;
}

void Chroma::ModifierViewController::DidActivate(bool first) {

  if (first) {
  }
}
