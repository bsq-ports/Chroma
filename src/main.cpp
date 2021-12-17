#include "questui/shared/QuestUI.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "main.hpp"

#include "Chroma.hpp"
#include "ChromaEvents.hpp"
#include "custom-types/shared/register.hpp"
#include "ChromaController.hpp"

#include "ChromaConfig.hpp"

#include <cstdlib>
#include "lighting/LightIDTableManager.hpp"

#include "pinkcore/shared/RequirementAPI.hpp"

#include "questui_components/shared/components/ViewComponent.hpp"
#include "questui_components/shared/components/ScrollableContainer.hpp"
#include "ui/ModifierViewController.hpp"

using namespace Chroma;
using namespace QuestUI;
using namespace QuestUI_Components;


Configuration& getConfig() {
    static Configuration config(modInfo);
    config.Load();
    return config;
}

Logger& getLogger() {
    static auto* logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}


void setChromaEnv() {
    setenv("DisableChromaReq", getChromaConfig().customColorEventsEnabled.GetValue() ? "0" : "1", true);
    if (getChromaConfig().customColorEventsEnabled.GetValue()) {
        PinkCore::RequirementAPI::RegisterInstalled("Chroma");
    } else {
        PinkCore::RequirementAPI::RemoveInstalled("Chroma");
    }

    PinkCore::RequirementAPI::RegisterInstalled("Chroma Lighting Events");
}

void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling){
    getLogger().info("DidActivate: %p, %d, %d, %d", self, firstActivation, addedToHierarchy, screenSystemEnabling);

    static ViewComponent *view;

    if(firstActivation) {
//        self->get_gameObject()->AddComponent<HMUI::Touchable *>();



        if (view) {
            delete view;
            view = nullptr;
        }

        view = new ViewComponent(self->get_transform(), {
            new ScrollableContainer({
                Chroma::UIUtils::buildMainUI<true>()
            })
        });
    }

    view->render();

}

extern "C" void setup(ModInfo& info) {
    info.id = modName;
    info.version = VERSION;
    modInfo = info;

    getConfig().Load();

    getChromaConfig().Init(info);

    getLogger().info("Completed Chroma setup!");
}

extern "C" void load() {
    il2cpp_functions::Init();
    QuestUI::Init();
    QuestUI::Register::RegisterModSettingsViewController(modInfo, DidActivate);
    QuestUI::Register::RegisterGameplaySetupMenu<ModifierViewController*>(modInfo, Register::Solo | Register::Online);

#if DEBUGB == 1
    #warning "Removing debug messages"
#else
    #warning "Debug messages hooray"
#endif

    //TODO: This might be redundant now because of the macro, keep it to be safe?
    // This disables the useless debug logs on the release build
#if DEBUGB == 1
    getLogger().debug("Disabling annoying loggers in release build");
    getLogger().DisableContext(Chroma::ChromaLogger::LightColorizer);
    getLogger().DisableContext(Chroma::ChromaLogger::ColorLightSwitch);
    getLogger().DisableContext(Chroma::ChromaLogger::EnvironmentRemoval);
    getLogger().DisableContext(Chroma::ChromaLogger::TrackLaneRings);
    getLogger().DisableContext(Chroma::ChromaLogger::ColorLightSwitch);
    getLogger().DisableContext(Chroma::ChromaLogger::ObjectDataDeserialize);
#endif

    getLogger().info("Installing types...");

    custom_types::Register::AutoRegister();

    getLogger().info("Installed types");

    getLogger().info("Installing Chroma hooks...");
    Chroma::Hooks::InstallHooks(getLogger());
    ChromaEvents::AddEventCallbacks(getLogger());
    getLogger().info("Installed Chroma hooks!");

    LightIDTableManager::InitTable();

    setChromaEnv();
}