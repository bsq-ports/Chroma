#pragma once

#include "questui_components/shared/context.hpp"
#include "questui_components/shared/RootContainer.hpp"
#include "questui_components/shared/components/ScrollableContainer.hpp"
#include "questui_components/shared/components/settings/ToggleSetting.hpp"
#include "questui_components/shared/components/Text.hpp"
#include "questui_components/shared/components/Image.hpp"
#include "questui_components/shared/components/ModifierButton.hpp"
#include "UnityEngine/MonoBehaviour.hpp"

#include <utility>
#include <vector>
#include <string>
#include <optional>
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include "main.hpp"

namespace Chroma {
namespace UIUtils {
UnityEngine::Sprite* configToIcon(ConfigUtils::ConfigValue<bool> const& configValue);

template <bool GameplayModifier, typename F, typename... TArgs>
auto chromaToggleUI(ConfigUtils::ConfigValue<bool>& configValue, F&& callable, TArgs&&... args) {
  using namespace QUC;

  if constexpr (GameplayModifier) {
    QUC::Image image(configToIcon(configValue), { 0, 0 });
    return ConfigUtilsModifierToggleSetting(configValue, std::forward<F>(callable), std::forward<TArgs>(args)...,
                                            image);
  } else {
    return ConfigUtilsToggleSetting(configValue, callable, std::forward<TArgs>(args)...);
  }
}

template <bool GameplayModifier> auto buildMainUI() {
  using namespace QUC;

  return Container(chromaToggleUI<GameplayModifier>(getChromaConfig().environmentEnhancementsEnabled, nullptr),
                   chromaToggleUI<GameplayModifier>(
                       getChromaConfig().customColorEventsEnabled,
                       [](auto&, bool, UnityEngine::Transform*, QUC::RenderContext&) { setChromaEnv(); }),
                   chromaToggleUI<GameplayModifier>(
                       getChromaConfig().customNoteColors,
                       [](auto&, bool, UnityEngine::Transform*, QUC::RenderContext&) { setChromaEnv(); }));
}
} // namespace UIUtils
} // namespace Chroma

DECLARE_CLASS_CODEGEN(Chroma, ModifierViewController, UnityEngine::MonoBehaviour, private
                      : QUC::RenderContext ctx{ nullptr };

                      DECLARE_INSTANCE_METHOD(void, DidActivate, bool first);

                      public
                      : DECLARE_SIMPLE_DTOR();)
