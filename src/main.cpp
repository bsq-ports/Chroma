#include "bsml/shared/BSML.hpp"
#include "main.hpp"

#include "Chroma.hpp"
#include "ChromaLogger.hpp"
#include "ChromaEvents.hpp"
#include "custom-types/shared/register.hpp"
#include "ChromaController.hpp"

#include "ChromaConfig.hpp"

#include <cstdlib>
#include "lighting/LightIDTableManager.hpp"

#include "songcore/shared/Capabilities.hpp"

#include "ui/ModifierViewController.hpp"

#include "environment_enhancements/EnvironmentMaterialManager.hpp"

#include "bsml/shared/BSML/SharedCoroutineStarter.hpp"

#include "bsml/shared/BSML/MainThreadScheduler.hpp"

using namespace Chroma;
//using namespace QuestUI;
//using namespace QUC;

Configuration& getConfig() {
  static Configuration config(modInfo);
  config.Load();
  return config;
}

void setChromaEnv() {
  setenv("DisableChromaReq", getChromaConfig().customColorEventsEnabled.GetValue() ? "0" : "1", true);
  if (getChromaConfig().customColorEventsEnabled.GetValue()) {
    SongCore::API::Capabilities::RegisterCapability("Chroma");
  } else {
    SongCore::API::Capabilities::UnregisterCapability("Chroma");
  }

  SongCore::API::Capabilities::RegisterCapability("Chroma Lighting Events");
}

void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
  ChromaLogger::Logger.info("DidActivate: {}, {}, {}, {}", fmt::ptr(self), static_cast<int>(firstActivation),
                   static_cast<int>(addedToHierarchy), static_cast<int>(screenSystemEnabling));

}

extern "C" void setup(CModInfo* info) {
  info->id = modName.c_str();
  info->version = VERSION;
  info->version_long = 0;

  getConfig().Load();

  getChromaConfig().Init({modName, VERSION, 0});

  ChromaLogger::Logger.info("Completed Chroma setup!");
}

extern "C" void late_load() {
  il2cpp_functions::Init();

  //QuestUI::Init();
  BSML::Register::RegisterSettingsMenu(modName, DidActivate, false);
  //QuestUI::Register::RegisterGameplaySetupMenu<ModifierViewController*>(modInfo, Register::Solo | Register::Online);

  ChromaLogger::Logger.info("Installing types...");

  custom_types::Register::AutoRegister();

  ChromaLogger::Logger.info("Installed types");

  ChromaLogger::Logger.info("Installing Chroma hooks...");
  Chroma::Hooks::InstallHooks();
  ChromaEvents::AddEventCallbacks();
  ChromaLogger::Logger.info("Installed Chroma hooks!");

  LightIDTableManager::InitTable();

  setChromaEnv();
}