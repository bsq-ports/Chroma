#include <unordered_map>

#include "Chroma.hpp"
#include "ChromaEvents.hpp"

#include "GlobalNamespace/BeatmapCallbacksController.hpp"

namespace Chroma::Component {
void StartEvent(GlobalNamespace::BeatmapCallbacksController* callbackController,
                CustomJSONData::CustomEventData* customEventData,
                ChromaEvents::AnimateComponentEventData const& eventAD);
void UpdateCoroutines(GlobalNamespace::BeatmapCallbacksController* callbackController);
} // namespace Chroma::Component