#pragma once

#include "questui_components/shared/components/ViewComponent.hpp"
#include "questui_components/shared/components/ScrollableContainer.hpp"
#include "questui_components/shared/components/layouts/MultiComponentGroup.hpp"
#include "questui_components/shared/components/settings/ToggleSetting.hpp"
#include "questui_components/shared/components/Text.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#pragma region QuestUI
namespace QuestUI {
    UnityEngine::UI::Toggle* CreateModifierButton(UnityEngine::Transform* parent, std::u16string_view buttonText, bool currentValue, UnityEngine::Sprite* iconSprite, std::function<void(bool)> const& onClick = nullptr, UnityEngine::Vector2 anchoredPosition = {0,0});

    inline UnityEngine::UI::Toggle* CreateModifierButton(UnityEngine::Transform* parent, std::u16string_view buttonText, bool currentValue, std::function<void(bool)> const& onClick = nullptr, UnityEngine::Vector2 anchoredPosition = {0,0}) {
        return CreateModifierButton(parent, buttonText, currentValue, nullptr, onClick, anchoredPosition);
    }

    inline UnityEngine::UI::Toggle* CreateModifierButton(UnityEngine::Transform* parent, std::string_view buttonText, bool currentValue, std::function<void(bool)> const& onClick = nullptr, UnityEngine::Vector2 anchoredPosition = {0,0}) {
        return CreateModifierButton(parent, to_utf16(buttonText), currentValue, nullptr, onClick, anchoredPosition);
    }

    inline UnityEngine::UI::Toggle* CreateModifierButton(UnityEngine::Transform* parent, std::string_view buttonText, bool currentValue, UnityEngine::Sprite* iconSprite, std::function<void(bool)> const& onClick = nullptr, UnityEngine::Vector2 anchoredPosition = {0,0}) {
        return CreateModifierButton(parent, to_utf16(buttonText), currentValue, iconSprite, onClick, anchoredPosition);
    }
}
#pragma endregion

#include <vector>
#include <string>
#include <optional>
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include "main.hpp"

#pragma region QUC
namespace QuestUI_Components {

    using MutableToggleSettingsData = MutableSettingsData<bool>;

    // TODO: Somehow this causes game buttons to be wide. How to fix?
    class ModifierToggle : public BaseSetting<bool, ModifierToggle, MutableToggleSettingsData> {
    public:
        struct InitToggleSettingsData {
            UnityEngine::Vector2 anchoredPosition = {0,0};
            UnityEngine::Sprite* iconImage;
        };

        explicit ModifierToggle(std::string_view text, bool currentValue, OnCallback callback = nullptr,
                               std::optional<InitToggleSettingsData> toggleData = std::nullopt)
                : BaseSetting(text, currentValue, std::move(callback)),
                  toggleInitData(toggleData) {}

    protected:
        void update() override;
        Component* render(UnityEngine::Transform *parentTransform) override;

        // render time
        UnityEngine::UI::Toggle* uiToggle = nullptr;

        // Constructor time
        const std::optional<InitToggleSettingsData> toggleInitData;
    };



#if defined(AddConfigValue) || __has_include("config-utils/shared/config-utils.hpp")
    using ConfigUtilsModifierToggleSetting = ConfigUtilsSetting<bool, ModifierToggle>;
#endif
}
#pragma endregion

namespace Chroma {
    namespace UIUtils {
        UnityEngine::Sprite* configToIcon(ConfigUtils::ConfigValue<bool> const& configValue);

        template<bool GameplayModifier, typename... TArgs>
        auto chromaToggleUI(ConfigUtils::ConfigValue<bool>& configValue, TArgs&&... args) {
            using namespace QuestUI_Components;

            if constexpr(GameplayModifier) {
                ModifierToggle::InitToggleSettingsData initData;
                initData.iconImage = configToIcon(configValue);
                return new ConfigUtilsModifierToggleSetting(configValue, std::forward<TArgs>(args)..., initData);
            } else {
                return new ConfigUtilsToggleSetting(configValue, std::forward<TArgs>(args)...);
            }
        }

        template<bool GameplayModifier>
        QuestUI_Components::MultiComponentGroup* buildMainUI() {
            using namespace QuestUI_Components;

            return new MultiComponentGroup({
                    chromaToggleUI<GameplayModifier>(getChromaConfig().environmentEnhancementsEnabled, [](auto*, bool, UnityEngine::Transform*){}),
                    chromaToggleUI<GameplayModifier>(getChromaConfig().customColorEventsEnabled, [](auto*, bool, UnityEngine::Transform*){
                        setChromaEnv();
                    }),
                    chromaToggleUI<GameplayModifier>(getChromaConfig().customNoteColors, [](auto*, bool, UnityEngine::Transform*){
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
