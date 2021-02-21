#pragma once

#include "GlobalNamespace/BeatmapObjectSpawnController.hpp"
#include "GlobalNamespace/IAudioTimeSource.hpp"

#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

#include "System/Collections/Generic/IEnumerator_1.hpp"

#include "UnityEngine/WaitForEndOfFrame.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"

#include "main.hpp"

namespace Chroma {
    class ChromaController {
    private:
        static bool _ChromaIsActive;

    public:
        static bool ChromaIsActive();
        static bool DoColorizerSabers;

        inline static GlobalNamespace::BeatmapObjectSpawnController* BeatmapObjectSpawnController = nullptr;
        inline static GlobalNamespace::IAudioTimeSource* IAudioTimeSource = nullptr;

        static void ToggleChromaPatches(bool val);

        //internal
        static void InitChromaPatches();

        // internal
        static void OnActiveSceneChanged(UnityEngine::SceneManagement::Scene current, UnityEngine::SceneManagement::Scene _);

    };
}

DECLARE_CLASS_INTERFACES(Chroma, DelayedStartEnumerator, "System", "Object", sizeof(Il2CppObject),
il2cpp_utils::GetClassFromName("System.Collections", "IEnumerator"),

DECLARE_CTOR(ctor, GlobalNamespace::BeatmapObjectSpawnController *instance);

        DECLARE_INSTANCE_FIELD(Il2CppObject*, current);
        DECLARE_INSTANCE_FIELD(bool, hasWaited);

        DECLARE_INSTANCE_FIELD(GlobalNamespace::BeatmapObjectSpawnController *, instance);

        DECLARE_OVERRIDE_METHOD(bool, MoveNext, il2cpp_utils::FindMethod("System.Collections", "IEnumerator", "MoveNext"));
        DECLARE_OVERRIDE_METHOD(Il2CppObject*, get_Current, il2cpp_utils::FindMethod("System.Collections", "IEnumerator", "get_Current"));
        DECLARE_OVERRIDE_METHOD(void, Reset, il2cpp_utils::FindMethod("System.Collections", "IEnumerator", "Reset"));

        REGISTER_FUNCTION(DelayedStartEnumerator,
                getLogger().debug("Registering WaitThenStartEnumerator!");

            REGISTER_FIELD(current);
            REGISTER_FIELD(hasWaited);

            REGISTER_FIELD(instance);

            REGISTER_METHOD(ctor);

            REGISTER_METHOD(MoveNext);
            REGISTER_METHOD(get_Current);
            REGISTER_METHOD(Reset);
            )
)