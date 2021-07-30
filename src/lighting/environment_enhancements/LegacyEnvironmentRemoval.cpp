#include "main.hpp"
#include "lighting/environment_enhancements/LegacyEnvironmentRemoval.hpp"
#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"

using namespace Chroma;

void Chroma::LegacyEnvironmentRemoval::Init() {
    static auto contextLogger = getLogger().WithContext(ChromaLogger::EnvironmentRemoval);

    auto& dynDataWrapper = ChromaController::infoDatCopy;

    getLogger().debug("Environment data: %p", dynDataWrapper ? "true" : "false");

    if (dynDataWrapper) {
        rapidjson::Document& dynData = *dynDataWrapper;
        auto objectsToKillIt = dynData.FindMember(ENVIRONMENTREMOVAL.c_str());

        if (objectsToKillIt != dynData.MemberEnd()) {

            auto objectsToKill = objectsToKillIt->value.GetArray();
            getLogger().warning("Legacy Environment Removal Detected...");
            getLogger().warning("Please do not use Legacy Environment Removal for new maps as it is deprecated and its functionality in future versions of Chroma cannot be guaranteed");

            auto gameObjects = UnityEngine::Resources::FindObjectsOfTypeAll<UnityEngine::GameObject*>();

            for (auto& oValue : objectsToKill) {
                std::string s = oValue.GetString();
                if (s == "TrackLaneRing" || s == "BigTrackLaneRing") {

                    for (int i = 0; i < gameObjects->Length(); i++) {
                        UnityEngine::GameObject *n = gameObjects->get(i);

                        if (!n)
                            continue;

                        auto nName = to_utf8(csstrtostr(n->get_name()));
                        if (nName.find(s) != std::string::npos) {
                            if (s == "TrackLaneRing" && nName.find("Big") != std::string::npos)
                                continue;

                            debugSpamLog(contextLogger, "Setting %s to disabled", nName.c_str());
                            n->SetActive(false);
                        }
                    }

                } else {
                    for (int i = 0; i < gameObjects->Length(); i++) {
                        UnityEngine::GameObject *n = gameObjects->get(i);

                        if (!n)
                            continue;

                        auto gStrIl2 = n->get_name();
                        std::string gStr = gStrIl2 ? to_utf8(csstrtostr(gStrIl2)) : "";


                        auto scene = n->get_scene();

                        if (!scene)
                            continue;

                        auto sceneNameIl2 = scene.get_name();
                        std::string sceneName = sceneNameIl2 ? to_utf8(csstrtostr(sceneNameIl2)) : "";

                        bool sceneEnvironment = !sceneName.empty() &&
                                                sceneName.find("Environment") != std::string::npos;

                        bool sceneMenu = !sceneName.empty() &&
                                         sceneName.find("Menu") != std::string::npos;

                        if (sceneEnvironment && !sceneMenu && gStr.find(s) != std::string::npos) {
                            debugSpamLog(contextLogger, "Setting %s to disabled else check", gStr.c_str());
                            n->SetActive(false);
                        }
                    }
                }

            }
        }
    }
}
