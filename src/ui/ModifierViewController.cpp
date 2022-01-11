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

#pragma region QuestUI
using namespace GlobalNamespace;
using namespace UnityEngine::Events;

#include "UnityEngine/Resources.hpp"
#include "questui/shared/ArrayUtil.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/ExternalComponents.hpp"
#include "HMUI/HoverTextSetter.hpp"
#include "Polyglot/LocalizedTextMeshProUGUI.hpp"
#include "UnityEngine/Events/UnityAction_1.hpp"

#define MakeDelegate(DelegateType, varName) (il2cpp_utils::MakeDelegate<DelegateType>(classof(DelegateType), varName))

Toggle *QuestUI::CreateModifierButton(UnityEngine::Transform *parent, std::u16string_view buttonText, bool currentValue,
                                      UnityEngine::Sprite *iconSprite, std::function<void(bool)> const &onClick,
                                      UnityEngine::Vector2 anchoredPosition) {
//    static WeakPtrGO <GameplayModifierToggle> toggleTemplate;
//    if (!toggleTemplate)
//        toggleTemplate = QuestUI::ArrayUtil::First(Resources::FindObjectsOfTypeAll<GameplayModifierToggle *>(),
//                                                   [](GameplayModifierToggle * x) { return csstrtostr(x->get_name()) == u"InstaFail"; });
    static GameplayModifierToggle *toggleTemplate;
    if (!toggleTemplate || !toggleTemplate->m_CachedPtr)
        toggleTemplate = QuestUI::ArrayUtil::First(Resources::FindObjectsOfTypeAll<GameplayModifierToggle *>(),
                                                   [](GameplayModifierToggle *x) {
                                                       return csstrtostr(x->get_name()) == u"InstaFail";
                                                   });
    GameplayModifierToggle *baseModifier = Object::Instantiate((GameplayModifierToggle *) toggleTemplate, parent,
                                                               false);
    static auto baseModifierName = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("BSMLModifier");
    baseModifier->set_name(baseModifierName);

    GameObject *gameObject = baseModifier->get_gameObject();
    gameObject->SetActive(false);

    Object::Destroy(baseModifier);
    Object::Destroy(gameObject->GetComponent<HMUI::HoverTextSetter *>());
    Object::Destroy(gameObject->get_transform()->Find(il2cpp_utils::newcsstr("Multiplier"))->get_gameObject());

    GameObject *nameText = gameObject->get_transform()->Find(il2cpp_utils::newcsstr("Name"))->get_gameObject();
    TMPro::TextMeshProUGUI *text = nameText->GetComponent<TMPro::TextMeshProUGUI *>();
    text->set_text(il2cpp_utils::newcsstr(buttonText));

    auto *localizer = gameObject->GetComponentInChildren<Polyglot::LocalizedTextMeshProUGUI *>();
    if (localizer != nullptr)
        GameObject::Destroy(localizer);

    Image *image = gameObject->get_transform()->Find(il2cpp_utils::newcsstr("Icon"))->GetComponent<Image *>();

    auto externalComponents = gameObject->AddComponent<ExternalComponents *>();
    externalComponents->Add(text);


    if (iconSprite) {
        image->set_sprite(iconSprite);
        externalComponents->Add(image);
    } else {
        Object::Destroy(image);
    }

    getLogger().debug("toggle magic");
    auto toggle = gameObject->GetComponent<Toggle *>();
    toggle->onValueChanged = Toggle::ToggleEvent::New_ctor();
    toggle->set_interactable(true);
    toggle->set_isOn(currentValue);
    if (onClick)
        toggle->onValueChanged->AddListener(MakeDelegate(UnityAction_1<bool>*, onClick));

    getLogger().debug("anchor position");
    auto *rectTransform = gameObject->GetComponent<RectTransform *>();
    rectTransform->set_anchoredPosition(anchoredPosition);

    gameObject->SetActive(true);

    return toggle;
}
#pragma endregion

DEFINE_TYPE(Chroma, ModifierViewController)

#pragma region QUC
void ModifierToggle::update() {
    if (!rendered)
        throw std::runtime_error("Toggle setting component has not rendered!");

    BaseSetting::update();
    uiToggle->set_enabled(data.enabled);
    uiToggle->set_interactable(data.interactable);

    uiToggle->set_isOn(getValue());
}

QuestUI_Components::Component *ModifierToggle::render(UnityEngine::Transform *parentTransform) {
    using namespace QuestUI;
    CallbackWrapper callback = constructWrapperCallback(parentTransform);

    if (toggleInitData) {
        uiToggle = CreateModifierButton(parentTransform, data.text, getValue(), toggleInitData->iconImage, callback,
                                        toggleInitData->anchoredPosition);
    } else {
        uiToggle = CreateModifierButton(parentTransform, data.text, getValue(), callback);
    }

    transform = uiToggle->get_transform();

    // From QuestUI
    UnityEngine::GameObject *nameText = transform->Find(il2cpp_utils::newcsstr("Name"))->get_gameObject();
    uiText = nameText->GetComponent<TMPro::TextMeshProUGUI *>();


    markAsRendered();
    update();

    return this;
}
#pragma endregion

UnityEngine::Sprite *UIUtils::configToIcon(ConfigUtils::ConfigValue<bool> const &configValue) {
    static const std::unordered_map<ConfigUtils::ConfigValue<bool> const*, std::string> spriteNameMap = {
            {&getChromaConfig().environmentEnhancementsEnabled, "GlobalIcon"},
            {&getChromaConfig().customColorEventsEnabled,       "LightIcon"},
            {&getChromaConfig().customNoteColors,               "DisappearingArrows"}
    };
    static std::unordered_map<std::string_view, UnityEngine::Sprite*> spriteMap;

    auto const& name = spriteNameMap.at(&configValue);

    auto& sprite = spriteMap[name];

    if (!sprite || !sprite->m_CachedPtr) {
        sprite = QuestUI::ArrayUtil::First(Resources::FindObjectsOfTypeAll<Sprite*>(), [&name](Sprite* x) {
            return to_utf8(csstrtostr(x->get_name())) == name;
        });
    }

    return sprite;
}

void Chroma::ModifierViewController::DidActivate(bool first) {
    if (first) {
        ModifierContainer *modifierContainer;

        view = ViewComponent(get_transform(), {
                new BackgroundableContainer("round-rect-panel", {
                        new Backgroundable("round-rect-panel",
                           modifierContainer = new ModifierContainer({
                                UIUtils::buildMainUI<true>()
                         })
                        )
                })
        });

        view.render();

        modifierContainer->getTransform()->get_gameObject()->GetComponent<LayoutElement *>()->set_preferredWidth(65);
    } else {
        view.render();
    }


}
