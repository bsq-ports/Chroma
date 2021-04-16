#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "colorizer/LightColorizer.hpp"
#include "GlobalNamespace/ColorManager.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/ParticleSystemEventEffect.hpp"
#include "UnityEngine/WaitForEndOfFrame.hpp"
#include "utils/ChromaUtils.hpp"
#include "custom-types/shared/coroutine.hpp"

#include <experimental/coroutine>

#include "ChromaObjectData.hpp"
#include "lighting/LightColorManager.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace Chroma;
using namespace ChromaUtils;
using namespace custom_types::Helpers;

Coroutine WaitThenStart(ParticleSystemEventEffect* instance, BeatmapEventType eventType)
{
    co_yield reinterpret_cast<enumeratorT*>(UnityEngine::WaitForEndOfFrame::New_ctor());
    LightColorizer::LSEStart(instance, eventType);
}

MAKE_HOOK_OFFSETLESS(
        ParticleSystemEventEffect_Start,
        void,
        ParticleSystemEventEffect* self
) {
    ParticleSystemEventEffect_Start(self);

    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        return;
    }

    auto* coro = custom_types::Helpers::CoroutineHelper::New(WaitThenStart(self, self->colorEvent));

    self->StartCoroutine(reinterpret_cast<enumeratorT*>(coro));
}

MAKE_HOOK_OFFSETLESS(
        ParticleSystemEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger,
        void,
        ParticleSystemEventEffect* self,
        BeatmapEventData* beatmapEventData
) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        ParticleSystemEventEffect_Start(self);
        return;
    }

    if (beatmapEventData->type == self->colorEvent)
    {
        LightColorizer::SetLastValue(self, beatmapEventData->value);
        LightColorManager::ColorLightSwitch(self, beatmapEventData);
    }
}

void Chroma::Hooks::ParticleSystemEventEffect() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), ParticleSystemEventEffect_Start, il2cpp_utils::FindMethodUnsafe("", "ParticleSystemEventEffect", "Start", 0));
    INSTALL_HOOK_OFFSETLESS(getLogger(), ParticleSystemEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger, il2cpp_utils::FindMethodUnsafe("", "ParticleSystemEventEffect", "HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger", 1));
}