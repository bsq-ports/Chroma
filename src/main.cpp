#include "main.hpp"

#include "light_colorizer.hpp"

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

#include "GlobalNamespace/BeatmapObjectManager.hpp"
#include "GlobalNamespace/BeatmapObjectSpawnController.hpp"
#include "GlobalNamespace/LightSwitchEventEffect.hpp"
#include "UnityEngine/GameObject.hpp"

using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace UnityEngine::SceneManagement;

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

// Thanks to StackDoubleFlow for this!
/*MAKE_HOOK_OFFSETLESS(BeatmapObjectSpawnController_Start, void, BeatmapObjectSpawnController *self) {
    BeatmapObjectManager* beatmapObjectManager = reinterpret_cast<BeatmapObjectManager*>(self->beatmapObjectSpawner); 
    BeatmapObjectCallbackController* coreSetup = self->beatmapObjectCallbackController;

    std::vector<std::string> objectsToKill;

    objectsToKill.push_back("RocketCar");

    auto *gameObjects = Object::FindObjectsOfType<GameObject*>();
    for (std::string objectName : objectsToKill) {
        if (objectName == "TrackLaneRing" || objectName == "BigTrackLaneRing") {
            for (int i = 0; i < gameObjects->Length(); i++) {
                auto* gameObject = gameObjects->values[i];
                if (gameObject->get_name()->Contains(il2cpp_utils::createcsstr(objectName))) {
                    if (objectName == "TrackLaneRing" && gameObject->get_name()->Contains(il2cpp_utils::createcsstr("Big"))) {
                        continue;
                    }

                    gameObject->SetActive(false);
                }
            }
        } else {
            for (int i = 0; i < gameObjects->Length(); i++) {
                auto* gameObject = gameObjects->values[i];
                if (gameObject->get_name()->Contains(il2cpp_utils::createcsstr(objectName))) {
                    // TODO: Make sure we aren't removing the environment in menus.
                    gameObject->SetActive(false);
                }
            }
        }
    }

    BeatmapObjectSpawnController_Start(self);
}*/

/* LightSwitchEventEffect */

MAKE_HOOK_OFFSETLESS(LightSwitchEventEffect_Start, void, LightSwitchEventEffect* self) {
    getLogger().info("gdfgdf...");

    LightColorizer::LSEStart(self, self->event);

    getLogger().info("aa...");

    LightSwitchEventEffect_Start(self);

    getLogger().info("hjg...");
}

extern "C" void load() {
    il2cpp_functions::Init();

    getLogger().info("Installing Chroma hooks...");
    
    INSTALL_HOOK_OFFSETLESS(getLogger(), LightSwitchEventEffect_Start, il2cpp_utils::FindMethodUnsafe("", "LightSwitchEventEffect", "Start", 0));
    //INSTALL_HOOK_OFFSETLESS(getLogger(), BeatmapObjectSpawnController_Start, il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectSpawnController", "Start", 0));

    getLogger().info("Installed Chroma hooks!");
}