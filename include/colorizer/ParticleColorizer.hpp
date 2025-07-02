#pragma once


#include <unordered_map>
#include <array>

#include "GlobalNamespace/BasicBeatmapEventType.hpp"

#include "sombrero/shared/ColorUtils.hpp"

namespace GlobalNamespace {
class ParticleSystemEventEffect;
class SimpleColorSO;
class MultipliedColorSO;
} // namespace GlobalNamespace


namespace Chroma {
class ParticleColorizer {
private:
  // ParticleSystemEventEffect still doesn't support boost colors!
  static int const COLOR_FIELDS = 2;

  GlobalNamespace::ParticleSystemEventEffect* _particleSystemEventEffect;
  GlobalNamespace::BasicBeatmapEventType _eventType;

  std::unordered_map<int, SafePtrUnity<GlobalNamespace::SimpleColorSO>> _simpleColorSOs;
  std::unordered_map<int, SafePtrUnity<GlobalNamespace::MultipliedColorSO>> _multipliedColorSOs;
  std::unordered_map<int, SafePtrUnity<GlobalNamespace::MultipliedColorSO>> _multipliedHighlightColorSOs;

  ParticleColorizer(GlobalNamespace::ParticleSystemEventEffect* particleSystemEventEffect,
                    GlobalNamespace::BasicBeatmapEventType BasicBeatmapEventType);

  static std::unordered_set<std::shared_ptr<ParticleColorizer>>&
  GetOrCreateColorizerList(GlobalNamespace::BasicBeatmapEventType eventType);

  void OnLightColorChanged(GlobalNamespace::BasicBeatmapEventType eventType, std::array<Sombrero::FastColor, 4> colors);

  void InitializeSO(std::string const& id, int index, bool highlight = false);

public:
  static std::shared_ptr<ParticleColorizer> New(GlobalNamespace::ParticleSystemEventEffect* particleSystemEventEffect,
                                                GlobalNamespace::BasicBeatmapEventType BasicBeatmapEventType);

  inline static std::unordered_map<int, std::unordered_set<std::shared_ptr<ParticleColorizer>>> Colorizers;

  int PreviousValue{};

  void UnsubscribeEvent();

  static void Reset();

  // extensions
  static std::unordered_set<std::shared_ptr<ParticleColorizer>>&
  GetParticleColorizers(GlobalNamespace::BasicBeatmapEventType BasicBeatmapEventType) {
    return Colorizers[BasicBeatmapEventType.value__];
  }
};
} // namespace Chroma
