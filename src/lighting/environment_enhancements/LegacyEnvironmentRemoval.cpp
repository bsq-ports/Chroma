#include "main.hpp"
#include "lighting/environment_enhancements/LegacyEnvironmentRemoval.hpp"
#include "Chroma.hpp"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"

using namespace Chroma;

void Chroma::LegacyEnvironmentRemoval::Init(CustomJSONData::CustomBeatmapData* customBeatmap) {
    static auto contextLogger = getLogger().WithContext(ChromaLogger::EnvironmentRemoval);

    auto dynDataWrapper = customBeatmap->customData;

    if (dynDataWrapper->value) {
        rapidjson::Value& dynData = *dynDataWrapper->value;
        auto objectsToKillIt = dynData.FindMember(ENVIRONMENTREMOVAL.c_str());

        if (objectsToKillIt != dynData.MemberEnd()) {
            auto objectsToKill = dynData.GetObject();
            getLogger().warning("Legacy Environment Removal Detected...");
            getLogger().warning("Please do not use Legacy Environment Removal for new maps as it is deprecated and its functionality in future versions of Chroma cannot be guaranteed");

            auto gameObjects = UnityEngine::Resources::FindObjectsOfTypeAll<UnityEngine::GameObject*>();

            for (auto& oValue : objectsToKill) {
                std::string s = oValue.value.GetString();
                if (s == "TrackLaneRing" || s == "BigTrackLaneRing") {

                    for (int i = 0; i < gameObjects->Length(); i++) {
                        UnityEngine::GameObject *n = gameObjects->get(i);

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

                        auto gStr = to_utf8(csstrtostr(n->get_name()));

                        std::string sceneName = n->get_scene().get_name() ? to_utf8(
                                csstrtostr(n->get_scene().get_name())) : "";

                        auto sceneEnvironment = !sceneName.empty() &&
                                                sceneName.find("Environment") != std::string::npos;

                        auto sceneMenu = !sceneName.empty() &&
                                         sceneName.find("Menu") != std::string::npos;

                        if (gStr.find(s) != std::string::npos && sceneEnvironment && sceneMenu) {
                            debugSpamLog(contextLogger, "Setting %s to disabled else check", gStr.c_str());
                            n->SetActive(false);
                        }
                    }
                }

            }
        }
    }
}
