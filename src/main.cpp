#include "custom-types/shared/coroutine.hpp"
#include "questui/shared/QuestUI.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/ExternalComponents.hpp"
#include "HMUI/Touchable.hpp"
#include "main.hpp"

#include "Chroma.hpp"
#include "colorizer/LightColorizer.hpp"
#include "colorizer/ObstacleColorizer.hpp"
#include "colorizer/BombColorizer.hpp"
#include "colorizer/SaberColorizer.hpp"
#include "custom-types/shared/register.hpp"

#include "lighting/ChromaRingsRotationEffect.hpp"
#include "ChromaController.hpp"
#include "lighting/ChromaGradientController.hpp"

#include "ChromaConfig.hpp"

#include <cstdlib>
#include "lighting/LightIDTableManager.hpp"

using namespace Chroma;
using namespace QuestUI;


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
}

void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling){
    getLogger().info("DidActivate: %p, %d, %d, %d", self, firstActivation, addedToHierarchy, screenSystemEnabling);

    if(firstActivation) {
        self->get_gameObject()->AddComponent<HMUI::Touchable*>();
        UnityEngine::GameObject* container = BeatSaberUI::CreateScrollableSettingsContainer(self->get_transform());



        auto* textGrid = container;
//        textGrid->set_spacing(1);

        BeatSaberUI::CreateText(textGrid->get_transform(), "Chroma settings.");

        BeatSaberUI::CreateText(textGrid->get_transform(), "Settings are saved when changed.");

        BeatSaberUI::CreateText(textGrid->get_transform(), "Not all settings have been tested or implemented.");
        BeatSaberUI::CreateText(textGrid->get_transform(), "Please use with caution.");

//        buttonsGrid->set_spacing(1);

        auto* boolGrid = container;

//        BeatSaberUI::CreateText(boolGrid->get_transform(), "Preferences.", false);
        BeatSaberUI::AddHoverHint(AddConfigValueToggle(boolGrid->get_transform(), getChromaConfig().environmentEnhancementsEnabled)->get_gameObject(),"Toggles whether Chroma should modify the environment as specified by a map.");
        BeatSaberUI::AddHoverHint(
        BeatSaberUI::CreateToggle(boolGrid->get_transform(), getChromaConfig().customColorEventsEnabled.GetName(), getChromaConfig().customColorEventsEnabled.GetValue(),
        [](bool toggle) {
            getChromaConfig().customColorEventsEnabled.SetValue(toggle);
            setChromaEnv();
        })->get_gameObject(), "Disables custom colors for Chroma maps. In other words, disables Chroma");
    }
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

    custom_types::Register::RegisterTypes<
            Chroma::LSEColorManager,
            Chroma::ChromaGradientController,
            Chroma::OCColorManager,
            Chroma::BNCColorManager,
            Chroma::ChromaRingsRotationEffect,
            Chroma::ChromaRotationEffect
    >();

    getLogger().info("Installed types");

    getLogger().info("Installing Chroma hooks...");
    Chroma::InstallHooks();
    getLogger().info("Installed Chroma hooks!");

    LightIDTableManager::InitTable();

    setChromaEnv();
}