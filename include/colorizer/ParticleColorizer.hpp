#pragma once

#include <unordered_map>
#include <array>

#include "GlobalNamespace/BasicBeatmapEventType.hpp"

#include "colorizer/LightColorizer.hpp"
#include "lighting/ChromaLightSwitchEventEffect.hpp"
#include "sombrero/shared/ColorUtils.hpp"

#include "GlobalNamespace/MultipliedColorSO.hpp"

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
  std::optional<LightColorizer*> lightColorizer;

  SafePtrUnity<GlobalNamespace::MultipliedColorSO> _highlightColor0;
  SafePtrUnity<GlobalNamespace::MultipliedColorSO> _highlightColor1;
  SafePtrUnity<GlobalNamespace::MultipliedColorSO> _lightColor0;
  SafePtrUnity<GlobalNamespace::MultipliedColorSO> _lightColor1;

  GlobalNamespace::BasicBeatmapEventType _previousValue;

  ParticleColorizer(GlobalNamespace::ParticleSystemEventEffect* particleSystemEventEffect,
                    GlobalNamespace::BasicBeatmapEventType BasicBeatmapEventType);

  static std::unordered_set<std::shared_ptr<ParticleColorizer>>& GetOrCreateColorizerList(GlobalNamespace::BasicBeatmapEventType eventType);

  void Refresh();

  [[nodiscard]] Sombrero::FastColor GetHighlightColor(int beatmapEventValue, bool colorBoost) const;

  [[nodiscard]] Sombrero::FastColor GetNormalColor(int beatmapEventValue, bool colorBoost) const;

public:
  static std::shared_ptr<ParticleColorizer> New(GlobalNamespace::ParticleSystemEventEffect* particleSystemEventEffect,
                                                GlobalNamespace::BasicBeatmapEventType BasicBeatmapEventType);

  inline static std::unordered_map<int, std::unordered_set<std::shared_ptr<ParticleColorizer>>> Colorizers;

  int PreviousValue{};

  void AssignLightColorizer(LightColorizer* colorizer);

  void Callback(GlobalNamespace::BasicBeatmapEventData* beatmapEventData);

  static void Reset();

  // extensions
  static std::unordered_set<std::shared_ptr<ParticleColorizer>>&
  GetParticleColorizers(GlobalNamespace::BasicBeatmapEventType BasicBeatmapEventType) {
    return Colorizers[BasicBeatmapEventType.value__];
  }
};
} // namespace Chroma
