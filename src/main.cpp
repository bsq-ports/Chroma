#include "main.hpp"

// Standard library includes
#include <cstdlib>

// Third-party library includes
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp" // Required for ChromaLogger
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "bsml/shared/BSML.hpp"
#include "bsml/shared/BSML/GameplaySetup/MenuType.hpp"
#include "bsml/shared/BSML/SharedCoroutineStarter.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"
#include "custom-types/shared/register.hpp"
#include "songcore/shared/Capabilities.hpp"

// Project-specific includes
#include "Chroma.hpp"
#include "ChromaLogger.hpp"
#include "ChromaEvents.hpp"
#include "ChromaController.hpp"
#include "ChromaConfig.hpp"
#include "lighting/LightIDTableManager.hpp"
#include "ui/ModifierViewController.hpp"
#include "environment_enhancements/EnvironmentMaterialManager.hpp"

using namespace Chroma;

void setChromaEnv() {
  setenv("DisableChromaReq", getChromaConfig().customColorEventsEnabled.GetValue() ? "0" : "1", true);
  if (getChromaConfig().customColorEventsEnabled.GetValue()) {
    SongCore::API::Capabilities::RegisterCapability("Chroma");
  } else {
    SongCore::API::Capabilities::UnregisterCapability("Chroma");
  }

  SongCore::API::Capabilities::RegisterCapability("Chroma Lighting Events");
}

extern "C" void setup(CModInfo* info) {
  info->id = modName.c_str();
  info->version = VERSION;
  info->version_long = 0;

  ChromaConfig_t::Init({ modName, VERSION, 0 });

  ChromaLogger::Logger.info("Completed Chroma setup!");
}

extern "C" void late_load() {
  il2cpp_functions::Init();

  BSML::Register::RegisterMainMenu<ModifierViewController*>("Chroma", "Chroma", "Colors!");
  // BSML::Register::RegisterGameplaySetupTab<ModifierViewController*>("Chroma", BSML::MenuType::Solo);

  ChromaLogger::Logger.info("Installing types...");

  custom_types::Register::AutoRegister();

  ChromaLogger::Logger.info("Installed types");

  // Force load to ensure order
  auto cjdModInfo = CustomJSONData::modInfo.to_c();
  auto tracksModInfo = CModInfo{ .id = "Tracks" };
  modloader_require_mod(&cjdModInfo, CMatchType::MatchType_IdOnly);
  modloader_require_mod(&tracksModInfo, CMatchType::MatchType_IdOnly);

  ChromaLogger::Logger.info("Installing Chroma hooks...");
  Chroma::Hooks::InstallHooks();
  ChromaEvents::AddEventCallbacks();
  ChromaLogger::Logger.info("Installed Chroma hooks!");

  LightIDTableManager::InitTable();

  setChromaEnv();
}
