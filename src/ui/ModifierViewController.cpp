#include "ui/ModifierViewController.hpp"

#include "HMUI/Touchable.hpp"

#include "questui_components/shared/components/layouts/GridLayoutGroup.hpp"

#include "main.hpp"

using namespace Chroma;
using namespace QuestUI_Components;

DEFINE_TYPE(Chroma, ModifierViewController)

void Chroma::ModifierViewController::DidActivate(bool first) {
    if (first) {
        get_gameObject()->AddComponent<HMUI::Touchable*>();


        view = ViewComponent(get_transform(), {
            new GridLayoutGroup({
                    UIUtils::buildMainUI<false>()
                })
            });

        view.render();
    }
}
