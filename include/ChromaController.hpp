#pragma once

#include "utils/CoroutineHelper.hpp"
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
        inline static bool _ChromaIsActive = false;

    public:
        static bool ChromaIsActive();
        static bool DoColorizerSabers();

        inline static GlobalNamespace::BeatmapObjectSpawnController* BeatmapObjectSpawnController = nullptr;
        inline static GlobalNamespace::IAudioTimeSource* IAudioTimeSource = nullptr;

        static void ToggleChromaPatches(bool val);

        // internal
        static void OnActiveSceneChanged(UnityEngine::SceneManagement::Scene current, UnityEngine::SceneManagement::Scene _);

        static std::generator<const void*> DelayedStartEnumerator(GlobalNamespace::BeatmapObjectSpawnController *beatmapObjectSpawnController);
    };
}