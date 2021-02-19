#include "main.hpp"

#include "Chroma.hpp"

static ModInfo modInfo;

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
    info.id = ID;
    info.version = VERSION;
    modInfo = info;

    getConfig().Load();
    getLogger().info("Completed Chroma setup!");
}

extern "C" void load() {
    il2cpp_functions::Init();

    getLogger().info("Installing Chroma hooks...");
    Chroma::InstallHooks();
    getLogger().info("Installed Chroma hooks!");
}