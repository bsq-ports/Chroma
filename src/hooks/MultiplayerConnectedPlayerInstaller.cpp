#include "Chroma.hpp"
#include "ChromaController.hpp"
#include "hooks/MultiplayerConnectedPlayerInstaller.hpp"

#include "GlobalNamespace/MultiplayerConnectedPlayerInstaller.hpp"
#include "GlobalNamespace/BeatmapDataTransformHelper.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/PracticeSettings.hpp"
#include "GlobalNamespace/EnvironmentEffectsFilterPreset.hpp"
#include "GlobalNamespace/EnvironmentIntensityReductionOptions.hpp"

#include "lighting/ChromaEventData.hpp"
#include "ChromaObjectData.hpp"

#include "System/Diagnostics/StackTrace.hpp"

using namespace GlobalNamespace;
using namespace Chroma;

MAKE_HOOK_MATCH(MultiplayerConnectedPlayerInstaller_InstallBindings,
                &MultiplayerConnectedPlayerInstaller::InstallBindings, void,
                MultiplayerConnectedPlayerInstaller* self) {
  MultiplayerConnectedPlayerInstallerHookHolder::MultiplayerInvoked = true;
  MultiplayerConnectedPlayerInstaller_InstallBindings(self);
  MultiplayerConnectedPlayerInstallerHookHolder::MultiplayerInvoked = false;
}

void MultiplayerConnectedPlayerInstallerHook(Logger& /*logger*/) {
  INSTALL_HOOK(getLogger(), MultiplayerConnectedPlayerInstaller_InstallBindings);
}

// ChromaInstallHooks(MultiplayerConnectedPlayerInstallerHook)