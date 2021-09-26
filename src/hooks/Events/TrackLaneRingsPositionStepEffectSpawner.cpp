#include "ChromaController.hpp"
#include "main.hpp"
#include "Chroma.hpp"

#include "lighting/ChromaEventData.hpp"

#include "GlobalNamespace/TrackLaneRingsPositionStepEffectSpawner.hpp"
#include "GlobalNamespace/BeatmapEventData.hpp"
#include "GlobalNamespace/TrackLaneRingsManager.hpp"
#include "GlobalNamespace/TrackLaneRing.hpp"

using namespace Chroma;
using namespace GlobalNamespace;

static float GetPrecisionStep(float const& defaultF, GlobalNamespace::BeatmapEventData* beatmapEventData)
{
    auto map = ChromaEventDataManager::ChromaEventDatas;
    auto it = map.find(beatmapEventData);

    if (it != map.end()) {
        auto chromaData = it->second;

        if (chromaData->Step) {
            return chromaData->Step.value();
        }
    }

    return defaultF;
}

static float GetPrecisionSpeed(float const& defaultF, GlobalNamespace::BeatmapEventData* beatmapEventData)
{
    auto map = ChromaEventDataManager::ChromaEventDatas;
    auto it = map.find(beatmapEventData);

    if (it != map.end()) {
        auto chromaData = it->second;

        if (chromaData->Speed) {
            return chromaData->Speed.value();
        }
    }

    return defaultF;
}

// Aero why do you have to use transpilers for everything damn it? Just rewrite the method
MAKE_HOOK_MATCH(TrackLaneRingsPositionStepEffectSpawner_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger,
                &TrackLaneRingsPositionStepEffectSpawner::HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger,
                void,
                     GlobalNamespace::TrackLaneRingsPositionStepEffectSpawner* self,
                     GlobalNamespace::BeatmapEventData* beatmapEventData) {
    // Essentially, here we cancel the original method. DO NOT call it IF it's a Chroma map
    if (!ChromaController::DoChromaHooks()) {
        TrackLaneRingsPositionStepEffectSpawner_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
        return;
    }

    if (beatmapEventData->type != self->beatmapEventType)
    {
        return;
    }
    float num = self->prevWasMinStep ? self->maxPositionStep : self->minPositionStep;

    num = GetPrecisionStep(num, beatmapEventData);

    self->prevWasMinStep = !self->prevWasMinStep;
    auto rings = self->trackLaneRingsManager->rings;
    for (int i = 0; i < rings->Length(); i++)
    {
        float destPosZ = (float)i * num;
        static auto SetPosition = FPtrWrapper<&GlobalNamespace::TrackLaneRing::SetPosition>::get();

        float moveSpeed = GetPrecisionSpeed(self->moveSpeed, beatmapEventData);
        SetPosition(rings->get(i), destPosZ, self->moveSpeed);
    }

}

void TrackLaneRingsPositionStepEffectSpawnerHook(Logger& logger) {
    INSTALL_HOOK(logger, TrackLaneRingsPositionStepEffectSpawner_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger);
//    INSTALL_HOOK_OFFSETLESS(getLogger(), SaberManager_Finalize, il2cpp_utils::FindMethodUnsafe("System", "Object", "Finalize", 0));
}

ChromaInstallHooks(TrackLaneRingsPositionStepEffectSpawnerHook)