#include "Chroma.hpp"
#include "ChromaController.hpp"
#include "hooks/MultiplayerConnectedPlayerInstaller.hpp"


#include "GlobalNamespace/BeatmapDataTransformHelper.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/PracticeSettings.hpp"
#include "GlobalNamespace/EnvironmentEffectsFilterPreset.hpp"
#include "GlobalNamespace/EnvironmentIntensityReductionOptions.hpp"

#include "lighting/ChromaEventData.hpp"
#include "ChromaEvents.hpp"
#include "ChromaObjectData.hpp"

#include "System/Diagnostics/StackTrace.hpp"


using namespace GlobalNamespace;
using namespace Chroma;

MAKE_HOOK_MATCH(BeatmapDataTransformHelper_CreateTransformedBeatmapData,&BeatmapDataTransformHelper::CreateTransformedBeatmapData, GlobalNamespace::IReadonlyBeatmapData*,
                ::GlobalNamespace::IReadonlyBeatmapData* beatmapData,
                ::GlobalNamespace::IPreviewBeatmapLevel* beatmapLevel,
                ::GlobalNamespace::GameplayModifiers* gameplayModifiers,
                bool leftHanded, ::GlobalNamespace::EnvironmentEffectsFilterPreset environmentEffectsFilterPreset,
                ::GlobalNamespace::EnvironmentIntensityReductionOptions* environmentIntensityReductionOptions,
                ::GlobalNamespace::MainSettingsModelSO* mainSettingsModel) {
    auto result = BeatmapDataTransformHelper_CreateTransformedBeatmapData(beatmapData, beatmapLevel, gameplayModifiers, leftHanded, environmentEffectsFilterPreset, environmentIntensityReductionOptions, mainSettingsModel);

    // Essentially, here we cancel the original method. DO NOT call it IF it's a Chroma map
    if (!ChromaController::DoChromaHooks() || MultiplayerConnectedPlayerInstallerHookHolder::MultiplayerInvoked) {
        return result;
    }

//    auto* stackTrace = System::Diagnostics::StackTrace::New_ctor();
//
//    getLogger().debug("Frames: %d", stackTrace->frames->Length());

//    if (!stackTrace->GetFrame(2)->GetMethod()->get_Name()->Contains(il2cpp_utils::newcsstr("MultiplayerConnectedPlayerInstaller"))) {
        ChromaObjectDataManager::deserialize(result);
        ChromaEventDataManager::deserialize(result);
        ChromaEvents::deserialize(result);
//    }

    return result;
}

void BeatmapDataTransformHelperHook(Logger& logger) {
    INSTALL_HOOK(getLogger(), BeatmapDataTransformHelper_CreateTransformedBeatmapData);
}

ChromaInstallHooks(BeatmapDataTransformHelperHook)