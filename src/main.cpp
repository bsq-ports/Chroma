#include "main.hpp"

#include "Chroma.hpp"
#include "colorizer/LightColorizer.hpp"
#include "colorizer/SaberColorizer.hpp"
#include "custom-types/shared/register.hpp"

#include "ChromaController.hpp"
#include "ChromaGradientController.hpp"

#include "ChromaConfig.hpp"

using namespace Chroma;



Configuration& getConfig() {
    static Configuration config(modInfo);
    config.Load();
    return config;
}

Logger& getLogger() {
    static Logger* logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
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

    getLogger().info("Installing types...");

    CRASH_UNLESS(custom_types::Register::RegisterType<Chroma::LSEColorManager>());
    CRASH_UNLESS(custom_types::Register::RegisterType<Chroma::ChangeColorCoroutine>());
    CRASH_UNLESS(custom_types::Register::RegisterType<Chroma::ChromaGradientController>());
    CRASH_UNLESS(custom_types::Register::RegisterType<Chroma::DelayedStartEnumerator>());

    getLogger().info("Installed types");

    getLogger().info("Installing Chroma hooks...");
    Chroma::InstallHooks();
    getLogger().info("Installed Chroma hooks!");
}