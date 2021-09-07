#include "ui/ModifierViewController.hpp"

#include "HMUI/Touchable.hpp"

#include "UnityEngine/RectOffset.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"

#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"

#include "questui_components/shared/components/Backgroundable.hpp"
#include "questui_components/shared/components/layouts/GridLayoutGroup.hpp"

#include "main.hpp"

using namespace Chroma;
using namespace QuestUI_Components;
using namespace QuestUI::BeatSaberUI;
using namespace UnityEngine::UI;
using namespace UnityEngine;

DEFINE_TYPE(Chroma, ModifierViewController)

UnityEngine::UI::VerticalLayoutGroup *CreateModifierContainer(UnityEngine::Transform *parent) {
    UnityEngine::UI::VerticalLayoutGroup* group = CreateVerticalLayoutGroup(parent);

    group->set_padding(RectOffset::New_ctor(3, 3, 2, 2));
    group->set_childControlHeight(false);
    group->set_childForceExpandHeight(false);

    group->get_gameObject()->GetComponent<ContentSizeFitter*>()->set_horizontalFit(ContentSizeFitter::FitMode::Unconstrained);
    group->get_gameObject()->GetComponent<ContentSizeFitter*>()->set_verticalFit(ContentSizeFitter::FitMode::PreferredSize);

    RectTransform* rectTransform = group->get_rectTransform(); //group->get_transform()->get_parent()->get_gameObject()->GetComponent<RectTransform*>();
    rectTransform->set_anchoredPosition({0, 3});
    rectTransform->set_anchorMin(UnityEngine::Vector2(0.5f, 0.5f));
    rectTransform->set_anchorMax(UnityEngine::Vector2(0.5f, 0.5f));
    rectTransform->set_sizeDelta(UnityEngine::Vector2(54.0f, 0.0f));

    group->get_gameObject()->GetComponent<LayoutElement*>()->set_preferredWidth(65);


    return group;
}

namespace QuestUI_Components {
    class ModifierContainer : public BaseContainer {
    public:
        explicit ModifierContainer(std::initializer_list<ComponentWrapper> children) : BaseContainer(children) {}
        explicit ModifierContainer(std::vector<ComponentWrapper> children) : BaseContainer(children) {}

    protected:
        Component* render(UnityEngine::Transform *parentTransform) override {
            verticalLayoutGroup = CreateModifierContainer(parentTransform);

            transform = verticalLayoutGroup->get_transform();

            rendered = true;
            update();

            return this;
        }

        UnityEngine::UI::VerticalLayoutGroup* verticalLayoutGroup = nullptr;
    };
}

void Chroma::ModifierViewController::DidActivate(bool first) {
    if (first) {
        get_gameObject()->AddComponent<HMUI::Touchable*>();

//        layout->get_gameObject()->GetComponent<LayoutElement*>()->set_preferredWidth(65);

        view = ViewComponent(get_transform(), {
            new BackgroundableContainer("round-rect-panel", {
                new Backgroundable("round-rect-panel", {
                   new ModifierContainer({
                       new Text("QuestUI layouts are painful, why can't I\n center this"),
                       UIUtils::buildMainUI<false>()
                   })
                })
            })
        });
    }

    view.render();
}
