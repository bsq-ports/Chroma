#include "Chroma.hpp"
#include "ChromaController.hpp"
#include "ChromaLogger.hpp"

#include "hooks/MultiplayerConnectedPlayerInstaller.hpp"

#include "GlobalNamespace/BeatmapDataTransformHelper.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"
#include "GlobalNamespace/BeatmapLevel.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/EnvironmentEffectsFilterPreset.hpp"
#include "GlobalNamespace/EnvironmentIntensityReductionOptions.hpp"

#include "lighting/ChromaEventData.hpp"
#include "ChromaEvents.hpp"
#include "ChromaObjectData.hpp"

using namespace GlobalNamespace;
using namespace Chroma;

MAKE_HOOK_MATCH(BeatmapDataTransformHelper_CreateTransformedBeatmapData,
                &BeatmapDataTransformHelper::CreateTransformedBeatmapData, GlobalNamespace::IReadonlyBeatmapData*,
                ::GlobalNamespace::IReadonlyBeatmapData* beatmapData, ::GlobalNamespace::BeatmapLevel* beatmapLevel,
                ::GlobalNamespace::GameplayModifiers* gameplayModifiers, bool leftHanded,
                ::GlobalNamespace::EnvironmentEffectsFilterPreset environmentEffectsFilterPreset,
                ::GlobalNamespace::EnvironmentIntensityReductionOptions* environmentIntensityReductionOptions,
                ::ByRef<::BeatSaber::Settings::Settings> settings) {
  auto* result = BeatmapDataTransformHelper_CreateTransformedBeatmapData(
      beatmapData, beatmapLevel, gameplayModifiers, leftHanded, environmentEffectsFilterPreset,
      environmentIntensityReductionOptions, settings);

  // Essentially, here we cancel the original method. DO NOT call it IF it's a Chroma map
  if (!ChromaController::DoChromaHooks() || MultiplayerConnectedPlayerInstallerHookHolder::MultiplayerInvoked) {
    return result;
  }

  if (auto customBeatmap = il2cpp_utils::try_cast<CustomJSONData::CustomBeatmapData>(result)) {
    auto& beatmapAD = TracksAD::getBeatmapAD(customBeatmap.value()->customData);

    if (!beatmapAD.valid) {
      TracksAD::readBeatmapDataAD(*customBeatmap);
    }

    ChromaObjectDataManager::deserialize(*customBeatmap);
    ChromaEventDataManager::deserialize(*customBeatmap);
    ChromaEvents::deserialize(*customBeatmap);
  }

  //    auto* stackTrace = System::Diagnostics::StackTrace::New_ctor();
  //
  //    ChromaLogger::Logger.debug("Frames: {}", stackTrace->frames->Length());

  //    if
  //    (!stackTrace->GetFrame(2)->GetMethod()->get_Name()->Contains(il2cpp_utils::newcsstr("MultiplayerConnectedPlayerInstaller")))
  //    {

  //    }

  return result;
}

void BeatmapDataTransformHelperHook() {
  INSTALL_HOOK(ChromaLogger::Logger, BeatmapDataTransformHelper_CreateTransformedBeatmapData);
}

ChromaInstallHooks(BeatmapDataTransformHelperHook)