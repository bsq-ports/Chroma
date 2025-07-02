#include "main.hpp"
#include "environment_enhancements/LegacyEnvironmentRemoval.hpp"
#include "Chroma.hpp"
#include "ChromaController.hpp"
#include "ChromaLogger.hpp"


#include "UnityEngine/Resources.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"

using namespace Chroma;

void Chroma::LegacyEnvironmentRemoval::Init(CustomJSONData::CustomBeatmapData* /*customBeatmap*/) {

  auto const& objectsToKillOpt = ChromaController::environmentObjectsRemovalV2;

  ChromaLogger::Logger.debug("Environment data: {}", objectsToKillOpt ? "true" : "false");

  if (objectsToKillOpt) {
    auto const& objectsToKill = *objectsToKillOpt;

    ChromaLogger::Logger.warn("Legacy Environment Removal Detected...");
    ChromaLogger::Logger.warn("Please do not use Legacy Environment Removal for new maps as it is deprecated and its "
                              "functionality in future versions of Chroma cannot be guaranteed");

    auto gameObjects = UnityEngine::Resources::FindObjectsOfTypeAll<UnityEngine::GameObject*>();

    for (auto const& s : objectsToKill) {
      if (s == "TrackLaneRing" || s == "BigTrackLaneRing") {

        for (auto const& n : gameObjects) {
          if (n == nullptr || UnityW(n) == nullptr) {
            continue;
          }

          auto nName = static_cast<std::string>(n->get_name());
          if (nName.find(s) != std::string::npos) {
            if (s == "TrackLaneRing" && nName.find("Big") != std::string::npos) {
              continue;
            }

            debugSpamLog("Setting {} to disabled", nName);
            n->SetActive(false);
          }
        }

      } else {
        for (auto const& n : gameObjects) {
          if (n == nullptr || UnityW(n) == nullptr) {
            continue;
          }

          auto gStrIl2 = n->get_name();
          std::string gStr = gStrIl2 != nullptr ? static_cast<std::string>(gStrIl2) : "";

          auto scene = n->get_scene();

          if (!scene.IsValid()) {
            continue;
          }

          auto sceneNameIl2 = scene.get_name();
          std::string sceneName = sceneNameIl2 != nullptr ? sceneNameIl2 : "";

          bool sceneEnvironment = !sceneName.empty() && sceneName.find("Environment") != std::string::npos;

          bool sceneMenu = !sceneName.empty() && sceneName.find("Menu") != std::string::npos;

          if (sceneEnvironment && !sceneMenu && gStr.find(s) != std::string::npos) {
            debugSpamLog("Setting {} to disabled else check", gStr.c_str());
            n->SetActive(false);
          }
        }
      }
    }
  }
}
