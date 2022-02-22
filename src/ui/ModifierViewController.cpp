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
using namespace QUC;
using namespace QuestUI::BeatSaberUI;
using namespace UnityEngine::UI;
using namespace UnityEngine;

DEFINE_TYPE(Chroma, ModifierViewController)

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
            return x->get_name() == name;
        });
    }

    return sprite;
}

void Chroma::ModifierViewController::DidActivate(bool first) {

    static detail::BackgroundableContainer container("round-rect-panel",
                Backgroundable("round-rect-panel", true,
                    ModifierContainer(
                        UIUtils::buildMainUI<true>()
                    )
                )
            );

    if (first) {
        this->ctx = RenderContext(get_transform());

        auto ret = detail::renderSingle(container, ctx);

        auto layoutElement = ret->get_gameObject()->GetComponentInChildren<LayoutElement *>();
        layoutElement->set_preferredWidth(55);
        // am lazy, will add QUC components later
        auto verticalLayout = ret->get_gameObject()->GetComponentInChildren<VerticalLayoutGroup *>();
        verticalLayout->set_spacing(0.75f);
    } else {
        detail::renderSingle(container, ctx);
    }
}
