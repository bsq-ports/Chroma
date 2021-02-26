#include "custom-types/shared/coroutine.hpp"

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

    custom_types::Register::RegisterTypes<
            Chroma::LSEColorManager,
            Chroma::ChromaGradientController
    >();

    getLogger().info("Installed types");

    getLogger().info("Installing Chroma hooks...");
    Chroma::InstallHooks();
    getLogger().info("Installed Chroma hooks!");
}