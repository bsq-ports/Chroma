#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "GlobalNamespace/BeatmapObjectManager.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"
#include "GlobalNamespace/BeatmapLineData.hpp"
#include "GlobalNamespace/BeatmapObjectType.hpp"
#include "GlobalNamespace/BeatmapObjectData.hpp"
#include "GlobalNamespace/Saber.hpp"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/GameObject.hpp"

#include "System/Action.hpp"
#include "ChromaConfig.hpp"
#include "bs-utils/shared/utils.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"

#include "main.hpp"
#include "colorizer/NoteColorizer.hpp"
#include "colorizer/SaberColorizer.hpp"
#include "colorizer/ObstacleColorizer.hpp"
#include "colorizer/BombColorizer.hpp"
#include "colorizer/LightColorizer.hpp"
#include "LegacyLightHelper.hpp"
#include "custom-types/shared/coroutine.hpp"

#include <vector>

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;
using namespace custom_types::Helpers;

static bool hookInstalled = false;

MAKE_HOOK_OFFSETLESS(ChromaController_NoteCutEvent, void, BeatmapObjectManager *self, NoteController *noteController,
                     NoteCutInfo *noteCutInfo) {
    if (!ChromaController::DoChromaHooks()) {
        ChromaController_NoteCutEvent(self, noteController, noteCutInfo);
        return;
    }
    NoteColorizer::ColorizeSaber(noteController, noteCutInfo);
    ChromaController_NoteCutEvent(self, noteController, noteCutInfo);
}

custom_types::Helpers::Coroutine
ChromaController::DelayedStartEnumerator(GlobalNamespace::BeatmapObjectSpawnController *beatmapObjectSpawnController) {
    getLogger().debug("Waiting");
    co_yield reinterpret_cast<enumeratorT *>(CRASH_UNLESS(il2cpp_utils::New<UnityEngine::WaitForEndOfFrame *>()));
    getLogger().debug("Waited");

    Chroma::ChromaController::BeatmapObjectSpawnController = beatmapObjectSpawnController;
    auto *beatmapObjectManager = reinterpret_cast<BeatmapObjectManager *>(beatmapObjectSpawnController->beatmapObjectSpawner);
    BeatmapObjectCallbackController *coreSetup = beatmapObjectSpawnController->beatmapObjectCallbackController;
    Chroma::ChromaController::IAudioTimeSource = coreSetup->audioTimeSource;

    IReadonlyBeatmapData *beatmapData = coreSetup->beatmapData;

    if (!hookInstalled) {
        INSTALL_HOOK_OFFSETLESS(getLogger(), ChromaController_NoteCutEvent,
                                il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectManager", "HandleNoteWasCut", 2));
        hookInstalled = true;
    }

    if (getChromaConfig().lightshowModifier.GetValue()) {
        auto list = reinterpret_cast<Generic::List_1<IReadonlyBeatmapData *> *>(beatmapData->get_beatmapLinesData());
        for (int i = 0; i < list->items->Length(); i++) {
            auto *b = reinterpret_cast<BeatmapLineData *>(list->items->values[i]);

            auto beatList = Generic::List_1<BeatmapObjectData *>::New_ctor();

            for (int j = 0; j < b->beatmapObjectsData->items->Length(); j++) {
                auto o = b->beatmapObjectsData->items->values[j];
                if (o->get_beatmapObjectType() != BeatmapObjectType::Note)
                    beatList->Add(o);
            }

            b->beatmapObjectsData = beatList;
        }

        Array<Saber *> *sabers = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::Saber *>();

        for (int i = 0; i < sabers->Length(); i++) {
            Saber *saber = sabers->values[i];
            saber->get_gameObject()->SetActive(false);
        }


        bs_utils::Submission::disable(modInfo);

        if (getChromaConfig().playersPlace.GetValue()) {
            auto o = GameObject::Find(il2cpp_utils::createcsstr("PlayersPlace"));

            if (o)
                o->SetActive(false);
        }

        if (getChromaConfig().spectrograms.GetValue()) {
            auto o = GameObject::Find(il2cpp_utils::createcsstr("Spectrograms"));

            if (o)
                o->SetActive(false);
        }

        if (getChromaConfig().backColumns.GetValue()) {
            auto o = GameObject::Find(il2cpp_utils::createcsstr("BackColumns"));

            if (o)
                o->SetActive(false);
        }

        if (getChromaConfig().buildings.GetValue()) {
            auto o = GameObject::Find(il2cpp_utils::createcsstr("Buildings"));

            if (o)
                o->SetActive(false);
        }
    }

//    // TODO: DO WE NEED THIS?
//    if (DoChromaHooks())
//    {
//
//        auto realBeatmapData = reinterpret_cast<GlobalNamespace::BeatmapEventData*>(beatmapData);
//
//        if (il2cpp_functions::class_is_assignable_from(realBeatmapData->klass, classof(CustomJSONData::CustomBeatmapData *))) {
//        auto customBeatmap = reinterpret_cast<CustomJSONData::CustomBeatmapData *>(beatmapData);
//            if (getChromaConfig().environmentEnhancementsEnabled.GetValue()) {
//                // Spaghetti code below until I can figure out a better way of doing this
//                auto dynData = customBeatmap->customData;
//                auto objectsToKillMember = dynData->value->FindMember(ENVIRONMENTREMOVAL); // ->GetObject(); Trees.at(dynData, ENVIRONMENTREMOVAL);
//                if (objectsToKillMember != dynData->value->MemberEnd()) {
//                    auto objectsToKill = objectsToKillMember->value.GetArray();
//                    auto gameObjects = Resources::FindObjectsOfTypeAll<UnityEngine::GameObject*>();
//
//                    for (auto& object : objectsToKill) {
//                        auto s = object.GetString();
//
//                        if (strcmp(s,"TrackLaneRing") == 0 || strcmp(s, "BigTrackLaneRing") == 0) {
//
//                            for (int i = 0; i < gameObjects->Length(); i++) {
//                                auto go = gameObjects->values[i];
//
//
//                                if (to_utf8(csstrtostr(go->get_name())))
//                            }
//
//                        }
//                            foreach(GameObject
//                            n
//                                    in
//                            gameObjects.Where(obj = > obj.name.Contains(s)))
//                            {
//                                if (s == "TrackLaneRing" && n.name.Contains("Big")) {
//                                    continue;
//                                }
//
//                                n.SetActive(false);
//                            }
//                        } else {
//                            foreach(GameObject
//                            n
//                            in
//                            gameObjects
//                                    .Where(obj = > obj.name.Contains(s) && (obj.scene.name ?.Contains("Environment")
//                            ?? false) && (!obj.scene.name ?.Contains("Menu") ?? false)))
//                            {
//                                n.SetActive(false);
//                            }
//                        }
//                    }
//                }
//            }
//
//        }

    // TODO: DO WE NEED THIS?
    if (DoChromaHooks()) {
        auto customBeatmap = reinterpret_cast<CustomJSONData::CustomBeatmapData *>(beatmapData);
        if (customBeatmap->customData && customBeatmap->customData->value &&
            getChromaConfig().environmentEnhancementsEnabled.GetValue()) {
            // Spaghetti code below until I can figure out a better way of doing this
            auto &dynData = *customBeatmap->customData->value;
            getLogger().debug("Environment removal time");
            PrintJSONValue(dynData);

            // TODO: Ok here's the problem, this
            // is getting the custom data of the Expert+ data
            // I'm guessing we need info.dat somehow
            auto objectsToKill = dynData.FindMember(ENVIRONMENTREMOVAL);
            if (objectsToKill != dynData.MemberEnd()) {
                auto gameObjects = Resources::FindObjectsOfTypeAll<GameObject *>();

                for (auto it = objectsToKill->value.MemberBegin(); it != objectsToKill->value.MemberEnd(); it++) {
                    auto s = it->value.GetString();

                    if (strcmp(s, "TrackLaneRing") == 0 || strcmp(s, "BigTrackLaneRing") == 0) {
                        for (int i = 0; i < gameObjects->Length(); i++) {
                            UnityEngine::GameObject *n = gameObjects->values[i];

                            auto nName = to_utf8(csstrtostr(n->get_name()));
                            if (nName.find(s) != std::string::npos) {
                                if (strcmp(s, "TrackLaneRing") == 0 && nName.find("Big") != std::string::npos)
                                    continue;

                                getLogger().debug("Setting %s to disabled", nName.c_str());
                                n->SetActive(false);
                            }
                        }

                    } else {
                        for (int i = 0; i < gameObjects->Length(); i++) {
                            UnityEngine::GameObject *n = gameObjects->values[i];

                            auto gStr = to_utf8(csstrtostr(n->get_name()));

                            std::string sceneName = n->get_scene().get_name() ? to_utf8(csstrtostr(n->get_scene().get_name())) : "";

                            auto sceneEnvironment = !sceneName.empty() &&
                                                    sceneName.find("Environment") != std::string::npos;

                            auto sceneMenu = !sceneName.empty() &&
                                             sceneName.find("Menu") != std::string::npos;

                            if (gStr.find(s) != std::string::npos && sceneEnvironment && sceneMenu) {
                                getLogger().debug("Setting %s to disabled else check", gStr.c_str());
                                n->SetActive(false);
                            }
                        }
                    }
                }
            }
        }
    }

    auto list = reinterpret_cast<Generic::List_1<BeatmapEventData *> *>(beatmapData->get_beatmapEventsData());
    std::vector<GlobalNamespace::BeatmapEventData *> eventData;

    for (int i = 0; i < list->items->Length(); i++) {
        eventData.push_back(list->items->values[i]);
    }

    // please let me kill legacy
    LegacyLightHelper::Activate(eventData);

    co_return; // Reached end of coroutine
}


void ChromaController::OnActiveSceneChanged(UnityEngine::SceneManagement::Scene current,
                                            UnityEngine::SceneManagement::Scene _) {
    if (strcmp(to_utf8(csstrtostr(current.get_name())).c_str(), "GameCore") == 0) {
        LightColorizer::ClearLSEColorManagers();
        ObstacleColorizer::ClearOCColorManagers();
        BombColorizer::ClearBNCColorManagers();
        NoteColorizer::ClearCNVColorManagers();
        SaberColorizer::ClearBSMColorManagers();
    }
}

bool ChromaController::ChromaRequired() {
    // 1 is true
    auto reqVar = getenv("req_Chroma");
    auto sugVar = getenv("sug_Chroma");

    return (reqVar && strcmp(reqVar, "1") == 0) || (sugVar && strcmp(sugVar, "1") == 0);
}

bool ChromaController::DoColorizerSabers() {
    return getChromaConfig().customColorEventsEnabled.GetValue() && ChromaRequired();
}

void ChromaController::SetChromaLegacy(bool v) {
    _ChromaLegacy = v;
}

bool ChromaController::GetChromaLegacy() {
    return _ChromaLegacy;
}


bool ChromaController::DoChromaHooks() {
    return getChromaConfig().customColorEventsEnabled.GetValue() && (true || _ChromaLegacy || ChromaRequired());
}