#pragma once

#include "ChromaEvents.hpp" // Needed for AnimateComponentEventData

// Forward declarations
namespace CustomJSONData {
class CustomEventData;
}

namespace GlobalNamespace {
class BeatmapCallbacksController;
}


namespace Chroma::Component {
void StartEvent(GlobalNamespace::BeatmapCallbacksController* callbackController,
                CustomJSONData::CustomEventData* customEventData,
                ChromaEvents::AnimateComponentEventData const& eventAD);
void UpdateCoroutines(GlobalNamespace::BeatmapCallbacksController* callbackController);
} // namespace Chroma::Component