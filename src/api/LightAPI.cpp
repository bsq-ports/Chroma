#include "LightAPI.hpp"
#include "colorizer/LightColorizer.hpp"

#include "conditional-dependencies/shared/main.hpp"
#include "shared/utils.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;

// TODO: unsure of this

EXPOSE_API(getLightColorSafe, LightAPI::LSEData*, BasicBeatmapEventType mb) {

  auto* lse = LightColorizer::GetLightColorizer(mb);

  if (lse == nullptr) {
    return nullptr;
  }

  auto colors = lse->getColor();

  auto _lightColor0 = colors[0];
  auto _lightColor1 = colors[1];
  auto _lightColor0Boost = colors[2];
  auto _lightColor1Boost = colors[3];

  auto* lseData = new LightAPI::LSEData{ std::make_optional(_lightColor0), std::make_optional(_lightColor1),
                                         std::make_optional(_lightColor0Boost), std::make_optional(_lightColor1Boost) };

  return lseData;
}

EXPOSE_API(setLightColorSafe, bool, BasicBeatmapEventType mb, bool refresh, std::optional<LightAPI::LSEData> lseData) {
  std::array<std::optional<Sombrero::FastColor>, 4> colors =
      lseData
          ? std::array<std::optional<Sombrero::FastColor>, 4>{ lseData->_lightColor0, lseData->_lightColor1,
                                                               lseData->_lightColor0Boost, lseData->_lightColor1Boost }
          : std::array<std::optional<Sombrero::FastColor>, 4>{ std::nullopt, std::nullopt, std::nullopt, std::nullopt };

  auto* colorizer = LightColorizer::GetLightColorizer(mb);
  if (colorizer != nullptr) {
    colorizer->Colorize(refresh, colors);
    return true;
  }
  return false;
}

EXPOSE_API(setAllLightingColorsSafe, void, bool refresh, std::optional<LightAPI::LSEData> data) {
  if (data) {
    LightColorizer::GlobalColorize(refresh, { data.value()._lightColor0, data.value()._lightColor1,
                                              data.value()._lightColor0Boost, data.value()._lightColor1Boost });
  } else {
    LightColorizer::GlobalColorize(refresh, { std::nullopt, std::nullopt, std::nullopt, std::nullopt });
  }
}

// Returns array of ILightWithId*

// While it does create a pointer on the heap,
// the API side of this should move the elements back into a value type then delete this pointer.
using LightMap = std::vector<GlobalNamespace::ILightWithId*>;

EXPOSE_API(getLightsSafe, LightMap*, GlobalNamespace::LightSwitchEventEffect* lse) {
  auto vectorOrg = VList<ILightWithId*>(LightColorizer::GetLightColorizer(lse->event)->Lights);
  auto* vectorPtr = new LightMap(vectorOrg.begin(), vectorOrg.end());

  return vectorPtr;
}

// Returns 2d array of ILightWithId*
// While it does create a pointer on the heap,
// the API side of this should move the elements back into a value type then delete this pointer.

using LightPropMap = std::unordered_map<int, std::vector<GlobalNamespace::ILightWithId*>>;
// EXPOSE_API(, std::unordered_map<int, std::vector<GlobalNamespace::ILightWithId *>>,
// GlobalNamespace::LightSwitchEventEffect *lse) {
EXPOSE_API(getLightsPropagationGroupedSafe, LightPropMap*, GlobalNamespace::LightSwitchEventEffect* lse) {
  auto mapOrg = LightColorizer::GetLightColorizer(lse->event)->getLightsPropagationGrouped();
  auto* mapPtr = new std::unordered_map<int, std::vector<GlobalNamespace::ILightWithId*>>(std::move(mapOrg));

  return mapPtr;
}