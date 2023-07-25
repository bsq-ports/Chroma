#include "ui/ModifierViewController.hpp"

#include "UnityEngine/RectOffset.hpp"

#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"

#include "questui_components/shared/components/Backgroundable.hpp"
#include "questui_components/shared/components/layouts/ModifierContainer.hpp"
#include "questui_components/shared/reference_comp.hpp"

#include "main.hpp"

using namespace Chroma;
using namespace QUC;
using namespace QuestUI::BeatSaberUI;
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

  if (!sprite || !sprite->m_CachedPtr) {
    sprite = QuestUI::ArrayUtil::First(Resources::FindObjectsOfTypeAll<Sprite*>(),
                                       [&name](Sprite* x) { return x->get_name() == name; });
  }

  return sprite;
}

void Chroma::ModifierViewController::DidActivate(bool first) {

  static detail::ModifierContainer layout(UIUtils::buildMainUI<true>());

  static ModifyLayoutElement layoutElement(QUC::detail::refComp(layout));

  static detail::BackgroundableContainer container(
      "round-rect-panel", Backgroundable("round-rect-panel", true, QUC::RefComp(layoutElement)));

  if (first) {
    this->ctx = RenderContext(get_transform());

    layout.spacing = 0.75f;
    layoutElement.preferredWidth = 55;

    detail::renderSingle(container, ctx);
  } else {
    detail::renderSingle(container, ctx);
  }
}
