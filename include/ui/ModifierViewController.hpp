#pragma once

#include "questui_components/shared/components/ViewComponent.hpp"
#include "questui_components/shared/components/ScrollableContainer.hpp"
#include "questui_components/shared/components/layouts/MultiComponentGroup.hpp"
#include "questui_components/shared/components/settings/ToggleSetting.hpp"
#include "questui_components/shared/components/Text.hpp"
#include "UnityEngine/MonoBehaviour.hpp"

#include <vector>
#include <string>
#include <optional>
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include "main.hpp"

namespace Chroma {
    namespace UIUtils {
        template<bool HaveWarningText>
        QuestUI_Components::MultiComponentGroup* buildMainUI() {
            using namespace QuestUI_Components;

            std::vector<ComponentWrapper> warningText;

            if constexpr (HaveWarningText) {
                warningText = {
                        new Text("Chroma settings."),
                        new Text("Settings are saved when changed."),
                        new Text("Not all settings have been tested or implemented."),
                        new Text("Please use with caution.")
                };
            }

            return new MultiComponentGroup({
                    new MultiComponentGroup(warningText),
                    new ConfigUtilsToggleSetting(getChromaConfig().environmentEnhancementsEnabled),
                    new ConfigUtilsToggleSetting(getChromaConfig().customColorEventsEnabled, [](ToggleSetting*, bool, UnityEngine::Transform*){
                        setChromaEnv();
                    })
            });
        }
    }
}

DECLARE_CLASS_CODEGEN(Chroma, ModifierViewController, UnityEngine::MonoBehaviour,
  private:
      QuestUI_Components::ViewComponent view;

      DECLARE_INSTANCE_METHOD(void, DidActivate, bool first);

      public:
      DECLARE_SIMPLE_DTOR();
)
