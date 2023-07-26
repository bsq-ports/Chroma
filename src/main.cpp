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

#include "questui_components/shared/components/ScrollableContainer.hpp"
#include "questui_components/shared/components/Text.hpp"
#include "ui/ModifierViewController.hpp"

#include "environment_enhancements/EnvironmentMaterialManager.hpp"

#include "GlobalNamespace/SharedCoroutineStarter.hpp"

#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

using namespace Chroma;
using namespace QuestUI;
using namespace QUC;

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

void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
  getLogger().info("DidActivate: %p, %d, %d, %d", self, static_cast<int>(firstActivation),
                   static_cast<int>(addedToHierarchy), static_cast<int>(screenSystemEnabling));

  static QUC::RenderContext ctx{ nullptr };

  static QUC::ScrollableContainer container(Text("Chroma settings."), Text("Settings are saved when changed."),
                                            Text("Not all settings have been tested or implemented."),
                                            Text("Please use with caution."), Chroma::UIUtils::buildMainUI<false>());

  if (firstActivation) {
    ctx.destroyTree();
    ctx = RenderContext(self->get_transform());
  }

  detail::renderSingle(container, ctx);
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

  // TODO: This might be redundant now because of the macro, keep it to be safe?
  //  This disables the useless debug logs on the release build
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