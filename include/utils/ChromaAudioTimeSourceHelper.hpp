#pragma once

#include "GlobalNamespace/IAudioTimeSource.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"

namespace ChromaTimeSourceHelper {

static float getSongTimeChroma(GlobalNamespace::IAudioTimeSource* timeSource) {
  static auto* timeSyncControllerClass = classof(GlobalNamespace::AudioTimeSyncController*);
  auto* timeSourceObject = reinterpret_cast<Il2CppObject*>(timeSource);
  if (timeSourceObject->klass == timeSyncControllerClass) {
    auto* timeSyncController = reinterpret_cast<GlobalNamespace::AudioTimeSyncController*>(timeSource);
    return timeSyncController->_songTime;
  } else {
    return timeSource->get_songTime();
  }
}

} // namespace ChromaTimeSourceHelper