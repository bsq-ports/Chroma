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

#include "bs-utils/shared/utils.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"

#include "main.hpp"
#include "ChromaConfig.hpp"
#include "colorizer/NoteColorizer.hpp"
#include "LegacyLightHelper.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;


DEFINE_CLASS(Chroma::DelayedStartEnumerator);


void Chroma::DelayedStartEnumerator::ctor(BeatmapObjectSpawnController *instance) {
    this->instance = instance;
    this->current = nullptr;
    this->hasWaited = false;
}

Il2CppObject* Chroma::DelayedStartEnumerator::get_Current() {
    return current;
}

void Chroma::DelayedStartEnumerator::Reset() {}

bool Chroma::DelayedStartEnumerator::MoveNext() {
    if (!hasWaited) {
        current = WaitForEndOfFrame::New_ctor();
        hasWaited = true;
        return true; // Continue coroutine
    }

    Chroma::ChromaController::BeatmapObjectSpawnController = instance;
    BeatmapObjectManager *beatmapObjectManager = reinterpret_cast<BeatmapObjectManager*>(instance->beatmapObjectSpawner);
    BeatmapObjectCallbackController *coreSetup = instance->beatmapObjectCallbackController;
    Chroma::ChromaController::IAudioTimeSource = coreSetup->audioTimeSource;

    IReadonlyBeatmapData *beatmapData = coreSetup->beatmapData;

    //auto delegate = il2cpp_utils::MakeDelegate<System::Action_2<NoteController*, NoteCutInfo*>*>(classof(System::Action_2<NoteController*, NoteCutInfo*>*),(Il2CppObject*) nullptr, NoteColorizer::ColorizeSaber);

    // TODO: According to dan, this might cause crashes even though PC modders can do it just fine
    //beatmapObjectManager->remove_noteWasCutEvent(delegate);

    //beatmapObjectManager->add_noteWasCutEvent(delegate);

    if (getChromaConfig().lightshowModifier.GetValue()) {
        auto list = reinterpret_cast<Generic::List_1<IReadonlyBeatmapData *> *>(beatmapData->get_beatmapLinesData());
        for (int i = 0; i < list->items->Length(); i++) {
            BeatmapLineData *b = reinterpret_cast<BeatmapLineData*>(list->items->values[i]);

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

    // TODO: DO WE NEED THIS?
//    if (Harmony.HasAnyPatches(HARMONYID))
//    {

//        if (il2cpp_functions::class_is_assignable_from(beatmapData->klass, classof(CustomJSONData::CustomBeatmapData *))) {
//        auto customBeatmap = reinterpret_cast<CustomJSONData::CustomBeatmapData *>(beatmapData);
//            if (getChromaConfig().environmentEnhancementsEnabled.GetValue()) {
//                // Spaghetti code below until I can figure out a better way of doing this
//                auto dynData = customBeatmap->customData;
//                List<object> objectsToKill = Trees.at(dynData, ENVIRONMENTREMOVAL);
//                if (objectsToKill != null) {
//                    IEnumerable <GameObject> gameObjects = Resources.FindObjectsOfTypeAll<GameObject>();
//                    foreach(string
//                    s
//                    in
//                    objectsToKill.Cast<string>())
//                    {
//                        if (s == "TrackLaneRing" || s == "BigTrackLaneRing") {
//                            foreach(GameObject
//                            n
//                            in
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

//        }

    auto list = reinterpret_cast<Generic::List_1<BeatmapEventData *> *>(beatmapData->get_beatmapEventsData());
    std::vector<GlobalNamespace::BeatmapEventData *> eventData;

    for (int i = 0; i < list->items->Length(); i++) {
        eventData.push_back(list->items->values[i]);
    }

    // please let me kill legacy
    LegacyLightHelper::Activate(eventData);

    return false; // Reached end of coroutine
}

IEnumerator *Chroma::ChromaController::DelayedStart(GlobalNamespace::BeatmapObjectSpawnController *instance) {
    Chroma::DelayedStartEnumerator *coroutine = CRASH_UNLESS(il2cpp_utils::New<Chroma::DelayedStartEnumerator*>(instance));
    return reinterpret_cast<IEnumerator*>(coroutine);
}