#include "Chroma.hpp"
#include "ChromaController.hpp"
#include "hooks/MultiplayerConnectedPlayerInstaller.hpp"

#include "GlobalNamespace/BeatmapDataTransformHelper.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"
#include "GlobalNamespace/BeatmapLevel.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/EnvironmentEffectsFilterPreset.hpp"
#include "GlobalNamespace/DefaultEnvironmentEventsFactory.hpp"

#include "lighting/ChromaEventData.hpp"
#include "ChromaEvents.hpp"
#include "ChromaObjectData.hpp"

#include "songcore/shared/SongCore.hpp"

using namespace GlobalNamespace;
using namespace Chroma;

MAKE_HOOK_MATCH(DefaultEnvironmentEventsFactory_InsertDefaultEvents,
                &DefaultEnvironmentEventsFactory::InsertDefaultEvents, void,
                ::GlobalNamespace::BeatmapData* beatmapData) {

  // TODO: Check if chroma map before doing this
  // I DON'T KNOW WHY THIS EXISTS BUT IT WILL BREAK MAPS

  // This method causes v2 maps to have default lights on
  // and that's not good?

  return;
}

void DefaultEnvironmentEventsFactoryHook() {
  // INSTALL_HOOK(ChromaLogger::Logger, DefaultEnvironmentEventsFactory_InsertDefaultEvents);
}

ChromaInstallHooks(DefaultEnvironmentEventsFactoryHook)