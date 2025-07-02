#include <algorithm>

#include "ChromaConfig.hpp"
#include "Chroma.hpp"
#include "hooks/SceneTransition/SceneTransitionHelper.hpp"

#include "lighting/LegacyLightHelper.hpp"
#include "ChromaController.hpp"
#include "lighting/LightIDTableManager.hpp"

#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/BeatmapDifficultySerializedMethods.hpp"

#include "utils/ChromaUtils.hpp"

#include "sombrero/shared/linq_functional.hpp"

#include "paper2_scotland2/shared/string_convert.hpp"

using namespace CustomJSONData;
using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;
using namespace Sombrero::Linq::Functional;

void SceneTransitionHelper::Patch(SongCore::SongLoader::CustomBeatmapLevel* beatmapLevel,
                                  GlobalNamespace::BeatmapKey key, GlobalNamespace::EnvironmentInfoSO* environment) {
  if (beatmapLevel == nullptr) {
    BasicPatch(nullptr, key, nullptr);
    return;
  }

  BasicPatch(beatmapLevel, key, environment);
}

void SceneTransitionHelper::Patch(SongCore::SongLoader::CustomBeatmapLevel* beatmapLevel,
                                  GlobalNamespace::BeatmapKey key, GlobalNamespace::EnvironmentInfoSO* environment,
                                  OverrideEnvironmentSettings*& overrideEnvironmentSettings) {
  if (beatmapLevel == nullptr) {
    BasicPatch(nullptr, key, nullptr);
    return;
  }

  BasicPatch(beatmapLevel, key, environment);
}

bool SceneTransitionHelper::BasicPatch(SongCore::SongLoader::CustomBeatmapLevel* beatmapLevel,
                                       GlobalNamespace::BeatmapKey key,
                                       GlobalNamespace::EnvironmentInfoSO* environment) {
  ChromaLogger::Logger.debug("Basic Patch {}", fmt::ptr(beatmapLevel));
  ChromaController::TutorialMode = false;
  ChromaController::environmentObjectsRemovalV2 = std::nullopt;

  bool legacyOverride = false;

  if (beatmapLevel == nullptr) {
    return false;
  }

  ChromaLogger::Logger.debug("Getting Save Data");

  auto saveData = beatmapLevel->standardLevelInfoSaveDataV2;

  if (!saveData) {
    return false;
  }

  auto customSaveInfo = saveData.value()->CustomSaveDataInfo;

  if (!customSaveInfo) {
    return false;
  }

  ChromaLogger::Logger.debug("Getting Characteristic and diff");

  auto diff = customSaveInfo.value().get().TryGetCharacteristicAndDifficulty(
      key.beatmapCharacteristic->get_serializedName(), key.difficulty);

  if (!diff) {
    return false;
  }

  ChromaLogger::Logger.debug("Getting Requirements & Suggestions");

  auto requirements = diff->get().requirements;
  auto suggestions = diff->get().suggestions;

  bool chromaRequirement = false;

  chromaRequirement |= std::ranges::find(requirements, REQUIREMENTNAME) != requirements.end();
  chromaRequirement |= std::ranges::find(suggestions, REQUIREMENTNAME) != suggestions.end();

  ChromaLogger::Logger.debug("Setting environment. Chroma Required: {}", chromaRequirement);

  // Legacy environment grab from info.dat
  if (saveData.value()->difficultyBeatmapSets) {
    auto diffSaveMap =
        saveData.value()->difficultyBeatmapSets |
        Select([&](auto&& x) -> ::GlobalNamespace::StandardLevelInfoSaveData::DifficultyBeatmap* {
          // short circuit
          if (x == nullptr || !x->difficultyBeatmaps) {
            return nullptr;
          }

          return x->difficultyBeatmaps | FirstOrDefault([&](auto&& y) {
                   // short circuit
                   if (y == nullptr || !y->difficulty) {
                     return false;
                   }

                   BeatmapDifficulty mapDifficulty;
                   GlobalNamespace::BeatmapDifficultySerializedMethods::BeatmapDifficultyFromSerializedName(
                       y->difficulty, byref(mapDifficulty));

                   return mapDifficulty == key.difficulty && key.beatmapCharacteristic.isAlive() &&
                          x->beatmapCharacteristicName == key.beatmapCharacteristic->get_serializedName();
                 });
        }) |
        First([](auto x) { return x != nullptr; });
    auto* saveMap = diffSaveMap.value_or(nullptr);
    ChromaLogger::Logger.debug("Savemap", fmt::ptr(saveMap));
    auto* customDifficultyBeatmap =
        il2cpp_utils::try_cast<SongCore::CustomJSONData::CustomDifficultyBeatmap>(saveMap).value_or(nullptr);

    // handle environment v2 removal
    if (customDifficultyBeatmap && customDifficultyBeatmap->customData.has_value()) {
      auto customData = customDifficultyBeatmap->customData.value();
      auto objectsToKillIt = customData.get().FindMember(Chroma::NewConstants::V2_ENVIRONMENT_REMOVAL.data());

      if (objectsToKillIt != customData.get().MemberEnd()) {
        ChromaController::environmentObjectsRemovalV2 = std::vector<std::string>();

        auto objectsToKill = objectsToKillIt->value.GetArray();
        for (auto const& object : objectsToKill) {
          ChromaController::environmentObjectsRemovalV2->emplace_back(
              Paper::StringConvert::from_utf16(object.GetString()));
        }
      }
    }
  }

  if (environment == nullptr) {
    ChromaLogger::Logger.debug("Environment is null, this is not right!", chromaRequirement);

    return false;
  }

  LightIDTableManager::SetEnvironment(static_cast<std::string>(environment->get_serializedName()));

  ChromaLogger::Logger.debug("Environment set");

  // TODO: Chromalite

  ChromaController::SetChromaLegacy(legacyOverride);
  ChromaController::setChromaRequired(chromaRequirement);

  ChromaController::randomizerThing = XoshiroCpp::Xoshiro128PlusPlus();

  ChromaLogger::Logger.debug("Scene Patch Finished");

  return ChromaController::ChromaRequired();
}