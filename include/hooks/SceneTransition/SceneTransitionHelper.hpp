#pragma once

#include "GlobalNamespace/EnvironmentInfoSO.hpp"
#include "GlobalNamespace/BeatmapKey.hpp"
#include "GlobalNamespace/OverrideEnvironmentSettings.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"
#include "custom-json-data/shared/CustomBeatmapSaveDatav3.h"
#include "songcore/shared/SongLoader/CustomBeatmapLevel.hpp"

namespace Chroma {
class SceneTransitionHelper {
public:
  static void Patch(SongCore::SongLoader::CustomBeatmapLevel* beatmapLevel, GlobalNamespace::BeatmapKey key, GlobalNamespace::EnvironmentInfoSO* environment);
  static void Patch(SongCore::SongLoader::CustomBeatmapLevel* beatmapLevel, GlobalNamespace::BeatmapKey key, GlobalNamespace::EnvironmentInfoSO* environment, GlobalNamespace::OverrideEnvironmentSettings*& overrideEnvironmentSettings);
  static bool BasicPatch(SongCore::SongLoader::CustomBeatmapLevel* beatmapLevel, GlobalNamespace::BeatmapKey key, GlobalNamespace::EnvironmentInfoSO* environment);
};
} // namespace Chroma