#include "ui/ModifierViewController.hpp"

#include "HMUI/Touchable.hpp"

#include "UnityEngine/RectOffset.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"

#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"

#include "questui_components/shared/components/Backgroundable.hpp"
#include "questui_components/shared/components/layouts/ModifierContainer.hpp"

#include "main.hpp"

using namespace Chroma;
using namespace QuestUI_Components;
using namespace QuestUI::BeatSaberUI;
using namespace UnityEngine::UI;
using namespace UnityEngine;

DEFINE_TYPE(Chroma, ModifierViewController)

void Chroma::ModifierViewController::DidActivate(bool first) {
    if (first) {
        ModifierContainer* modifierContainer;

        view = ViewComponent(get_transform(), {
            new BackgroundableContainer("round-rect-panel", {
                new Backgroundable("round-rect-panel",
                   modifierContainer = new ModifierContainer({
                       UIUtils::buildMainUI<false>()
                   })
                )
            })
        });

        view.render();

        modifierContainer->getTransform()->get_gameObject()->GetComponent<LayoutElement*>()->set_preferredWidth(65);
    } else {
        view.render();
    }


}
