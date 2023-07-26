#include "main.hpp"
#include "environment_enhancements/LegacyEnvironmentRemoval.hpp"
#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"

using namespace Chroma;

void Chroma::LegacyEnvironmentRemoval::Init(CustomJSONData::CustomBeatmapData* /*customBeatmap*/) {
  static auto contextLogger = getLogger().WithContext(ChromaLogger::EnvironmentRemoval);

  auto& dynDataWrapper = ChromaController::infoDatCopy;

  getLogger().debug("Environment data: %p", dynDataWrapper ? "true" : "false");

  if (dynDataWrapper) {
    DocumentUTF16& dynData = *dynDataWrapper;
    auto objectsToKillIt = dynData.FindMember(Chroma::NewConstants::V2_ENVIRONMENT_REMOVAL.data());

    if (objectsToKillIt != dynData.MemberEnd()) {

      auto objectsToKill = objectsToKillIt->value.GetArray();
      getLogger().warning("Legacy Environment Removal Detected...");
      getLogger().warning("Please do not use Legacy Environment Removal for new maps as it is deprecated and its "
                          "functionality in future versions of Chroma cannot be guaranteed");

      auto gameObjects = UnityEngine::Resources::FindObjectsOfTypeAll<UnityEngine::GameObject*>();

      for (auto& oValue : objectsToKill) {
        std::u16string s = oValue.GetString();
        if (s == u"TrackLaneRing" || s == u"BigTrackLaneRing") {

          for (int i = 0; i < gameObjects.Length(); i++) {
            UnityEngine::GameObject* n = gameObjects.get(i);

            if (n == nullptr) {
              continue;
            }

            auto nName = csstrtostr(n->get_name());
            if (nName.find(s) != std::string::npos) {
              if (s == u"TrackLaneRing" && nName.find(u"Big") != std::string::npos) continue;

              debugSpamLog(contextLogger, "Setting %s to disabled", nName);
              n->SetActive(false);
            }
          }

        } else {
          for (int i = 0; i < gameObjects.Length(); i++) {
            UnityEngine::GameObject* n = gameObjects.get(i);

            if (n == nullptr) {
              continue;
            }

            auto gStrIl2 = n->get_name();
            std::u16string gStr = gStrIl2 != nullptr ? std::u16string(csstrtostr(gStrIl2)) : u"";

            auto scene = n->get_scene();

            if (!scene.IsValid()) {
              continue;
            }

            auto sceneNameIl2 = scene.get_name();
            std::string sceneName = sceneNameIl2 != nullptr ? sceneNameIl2 : "";

            bool sceneEnvironment = !sceneName.empty() && sceneName.find("Environment") != std::string::npos;

            bool sceneMenu = !sceneName.empty() && sceneName.find("Menu") != std::string::npos;

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
