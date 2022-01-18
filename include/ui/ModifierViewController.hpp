#pragma once

#include "questui_components/shared/context.hpp"
#include "questui_components/shared/RootContainer.hpp"
#include "questui_components/shared/components/ScrollableContainer.hpp"
#include "questui_components/shared/components/settings/ToggleSetting.hpp"
#include "questui_components/shared/components/Text.hpp"
#include "questui_components/shared/components/Image.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#pragma region QuestUI
namespace QuestUI::BeatSaberUI {
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

#include <utility>
#include <vector>
#include <string>
#include <optional>
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include "main.hpp"

#pragma region QUC
namespace QUC {

    // TODO: Somehow this causes game buttons to be wide. How to fix?
    struct ModifierToggle : public ToggleSetting {
    public:
        Image image;

        template<class F = OnCallback>
        ModifierToggle(Text const &txt, F &&callable, bool currentValue, Image image = Image(nullptr, {0, 0}))
                : ToggleSetting(txt,
                                callable,
                                currentValue),
                  image(std::move(image)) {}

        template<class F = OnCallback>
        ModifierToggle(std::string_view txt, F &&callable, bool currentValue, Image image = Image(nullptr, {0, 0}))
                : ToggleSetting(txt,
                                callable,
                                currentValue),
                  image(std::move(image)) {}

        template<class F = OnCallback>
        ModifierToggle(Text const &txt, F &&callable, bool currentValue,
                       bool enabled_ = true, bool interact = true,
                       std::optional<UnityEngine::Vector2> anch = std::nullopt, Image image = Image(nullptr, {0, 0}))
                : ToggleSetting(txt,
                                callable,
                                currentValue,
                                enabled_,
                                interact,
                                anch),
                  image(std::move(image)) {}

        template<class F = OnCallback>
        ModifierToggle(std::string_view const &txt, F &&callable, bool currentValue,
                       bool enabled_ = true, bool interact = true,
                       std::optional<UnityEngine::Vector2> anch = std::nullopt, Image image = Image(nullptr, {0, 0}))
                : ToggleSetting(txt,
                                callable,
                                currentValue,
                                enabled_,
                                interact,
                                anch),
                  image(std::move(image)) {}

        UnityEngine::Transform *render(RenderContext &ctx, RenderContextChildData &data) {
            auto &toggle = ctx.getChildData(ToggleSetting::key).getData<UnityEngine::UI::Toggle*>();
            auto &toggleText = ctx.getChildData(text->key).getData<TMPro::TextMeshProUGUI *>();
            auto &imageView = ctx.getChildData(image.key).getData<HMUI::ImageView *>();


            if (!toggle) {
                auto parent = &ctx.parentTransform;
                auto const &usableText = text ? text->text : *str;

                auto cbk = [this, callback = this->callback, parent, &ctx](bool val) {
                    toggleButton.value = val;
                    toggleButton.value.clear();
                    if (callback)
                        callback(*this, val, parent, ctx);
                };
                if (anchoredPosition) {
                    toggle = QuestUI::BeatSaberUI::CreateModifierButton(parent, usableText, *toggleButton.value,
                                                                        *image.sprite, cbk, *anchoredPosition);
                } else {
                    toggle = QuestUI::BeatSaberUI::CreateModifierButton(parent, usableText, *toggleButton.value,
                                                                        *image.sprite, cbk);
                }
                image.sprite.clear();
                toggleButton.value.clear();

                auto toggleTransform = toggle->get_transform();

                imageView = toggleTransform->Find(il2cpp_utils::newcsstr("Icon"))->GetComponent<HMUI::ImageView *>();
                toggleText = toggleTransform->Find(il2cpp_utils::newcsstr("Name"))->GetComponent<TMPro::TextMeshProUGUI *>();
            }

            return ToggleSetting::render(ctx, data);
        }
    };

    static_assert(renderable<ModifierToggle>);



#if defined(AddConfigValue) || __has_include("config-utils/shared/config-utils.hpp")
    using ConfigUtilsModifierToggleSetting = ConfigUtilsSetting<bool, ModifierToggle>;
#endif
}
#pragma endregion

namespace Chroma {
    namespace UIUtils {
        UnityEngine::Sprite* configToIcon(ConfigUtils::ConfigValue<bool> const& configValue);

        template<bool GameplayModifier, typename F, typename... TArgs>
        auto chromaToggleUI(ConfigUtils::ConfigValue<bool>& configValue, F&& callable, TArgs&&... args) {
            using namespace QUC;

            if constexpr(GameplayModifier) {
                QUC::Image image(configToIcon(configValue), {0,0});
//                return ConfigUtilsToggleSetting(configValue, callable, std::forward<TArgs>(args)...);
                return ConfigUtilsModifierToggleSetting(configValue, std::forward<F>(callable), std::forward<TArgs>(args)..., image);
            } else {
                return ConfigUtilsToggleSetting(configValue, callable, std::forward<TArgs>(args)...);
            }
        }

        template<bool GameplayModifier>
        auto buildMainUI() {
            using namespace QUC;

            return Container(
                    chromaToggleUI<GameplayModifier>(getChromaConfig().environmentEnhancementsEnabled, [](auto&, bool, UnityEngine::Transform *, QUC::RenderContext &){}),
                    chromaToggleUI<GameplayModifier>(getChromaConfig().customColorEventsEnabled, [](auto&, bool, UnityEngine::Transform *, QUC::RenderContext &){
                        setChromaEnv();
                    }),
                    chromaToggleUI<GameplayModifier>(getChromaConfig().customNoteColors, [](auto&, bool, UnityEngine::Transform *, QUC::RenderContext &){
                       setChromaEnv();
                   })
            );
        }
    }
}

DECLARE_CLASS_CODEGEN(Chroma, ModifierViewController, UnityEngine::MonoBehaviour,
  private:
      QUC::RenderContext ctx{nullptr};


      DECLARE_INSTANCE_METHOD(void, DidActivate, bool first);

      public:
      DECLARE_SIMPLE_DTOR();
)
