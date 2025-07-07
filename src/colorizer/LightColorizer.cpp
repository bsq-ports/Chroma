#include "Chroma.hpp"
#include "ChromaLogger.hpp"
#include "beatsaber-hook/shared/utils/typedefs-array.hpp"

#include "lighting/LightIDTableManager.hpp"
#include "colorizer/LightColorizer.hpp"

#include <algorithm>
#include <unordered_map>
#include <utility>

#include "GlobalNamespace/TrackLaneRingsManager.hpp"
#include "GlobalNamespace/TrackLaneRing.hpp"
#include "GlobalNamespace/ColorSO.hpp"
#include "GlobalNamespace/MultipliedColorSO.hpp"

#include "UnityEngine/Transform.hpp"

#include "System/Linq/Enumerable.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Sombrero;
using namespace Chroma;

LightColorizer::LightColorizer(ChromaLightSwitchEventEffect* lightSwitchEventEffect, LightWithIdManager* lightManager)
    : _lightSwitchEventEffect(lightSwitchEventEffect), lightId(lightSwitchEventEffect->_lightsID), _colors(COLOR_FIELDS),
      _originalColors() {

  auto Initialize = [this](UnityW<ColorSO> colorSO, int index) {
    if (auto mColor = il2cpp_utils::try_cast<MultipliedColorSO>(colorSO.ptr())) {
      auto lightSO = mColor.value()->_baseColor;
      _originalColors[index] = lightSO;
    } else if (auto sColor = il2cpp_utils::try_cast<SimpleColorSO>(colorSO.ptr())) {
      _originalColors[index] = sColor.value();
    }
  };

  Initialize(lightSwitchEventEffect->_lightColor0, 0);
  Initialize(lightSwitchEventEffect->_lightColor1, 1);
  Initialize(lightSwitchEventEffect->_lightColor0Boost, 2);
  Initialize(lightSwitchEventEffect->_lightColor1Boost, 3);

  // AAAAAA PROPAGATION STUFFF
  auto* lights = lightManager->_lights.get(lightSwitchEventEffect->_lightsID);

  // possible uninitialized
  if (lights == nullptr) {
    lights = System::Collections::Generic::List_1<::GlobalNamespace::ILightWithId*>::New_ctor(10);
    lightManager->_lights[lightSwitchEventEffect->_lightsID] = lights;
  }

  LightsSafePtr.emplace(reinterpret_cast<List<GlobalNamespace::ILightWithId*>*>(lights));
}

LightColorizer& LightColorizer::New(ChromaLightSwitchEventEffect* lightSwitchEventEffect,
                                    GlobalNamespace::LightWithIdManager* lightManager) {
  auto& light =
      Colorizers.emplace(lightSwitchEventEffect->_event.value__, LightColorizer(lightSwitchEventEffect, lightManager)).first->second;
  ColorizersByLightID[lightSwitchEventEffect->_lightsID] = &light;
  return light;
}

void LightColorizer::GlobalColorize(bool refresh, std::optional<std::vector<GlobalNamespace::ILightWithId*>> const& lights,
                                    LightColorOptionalPalette const& colors) {
  for (int i = 0; i < colors.size(); i++) {
    GlobalColor[i] = colors[i];
  }

  for (auto& [_, colorizer] : Colorizers) {
    // Allow light colorizer to not force color
    if (!refresh) {
      continue;
    }

    colorizer.Refresh(lights);
  }
}

void LightColorizer::Reset() {
  for (int i = 0; i < COLOR_FIELDS; i++) {
    GlobalColor[i] = std::nullopt;
  }
  Colorizers.clear();
  ColorizersByLightID.clear();
  _contracts.clear();
  _contractsByLightID.clear();
}

std::vector<ILightWithId*> LightColorizer::GetPropagationLightWithIds(std::vector<int> const& ids) {
  std::vector<ILightWithId*> result;
  auto const& props = getLightsPropagationGrouped();
  auto lightCount = props.size();
  for (int id : ids) {
    if (lightCount > id) {
      auto const& lights = props.at(id);

      std::ranges::copy(lights, std::back_inserter(result));
    }
  }

  return result;
}

std::vector<ILightWithId*> LightColorizer::GetLightWithIds(std::vector<int> const& ids) const {
  std::vector<ILightWithId*> result;
  result.reserve(ids.size());

  auto lights = getLights();
  auto maxLightId = lights.size();

  for (int id : ids) {
    // Transform
    id = LightIDTableManager::GetActiveTableValue(lightId, id).value_or(id);

    auto* lightWithId = id >= 0 && id < maxLightId ? lights[id] : nullptr;
    if (lightWithId != nullptr) {
      result.push_back(lightWithId);
    }
  }

  return result;
}

void LightColorizer::CreateLightColorizerContractByLightID(int lightId, std::function<void(LightColorizer&)> const& callback) {
  auto it = ColorizersByLightID.find(lightId);

  if (it != ColorizersByLightID.end()) {
    CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Light id {} now", lightId);
    callback(*it->second);
  } else {
    CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Fulfilling callback for light id {} later", lightId);
    _contractsByLightID.emplace_back(lightId, callback);
  }
}

void LightColorizer::CreateLightColorizerContract(BasicBeatmapEventType type, std::function<void(LightColorizer&)> const& callback) {
  auto it = Colorizers.find(type.value__);

  if (it != Colorizers.end()) {
    callback(it->second);
  } else {
    _contracts.emplace_back(type.value__, callback);
  }
}

void LightColorizer::CompleteContracts(ChromaLightSwitchEventEffect* chromaLightSwitchEventEffect) {
  // complete open contracts
  for (auto it = _contracts.begin(); it != _contracts.end();) {
    auto const& [type, callback] = *it;
    if (type != chromaLightSwitchEventEffect->_event.value__) {
      it++;
      continue;
    }

    callback(*chromaLightSwitchEventEffect->lightColorizer);
    it = _contracts.erase(it);
  }

  for (auto it = _contractsByLightID.begin(); it != _contractsByLightID.end();) {
    auto const& [lightId, callback] = *it;
    if (lightId != chromaLightSwitchEventEffect->_lightsID) {
      it++;
      continue;
    }

    callback(*chromaLightSwitchEventEffect->lightColorizer);
    it = _contractsByLightID.erase(it);
  }
}

std::unordered_map<int, std::vector<GlobalNamespace::ILightWithId*>> const& LightColorizer::getLightsPropagationGrouped() {
  if (LightsPropagationGrouped) {
    return *LightsPropagationGrouped;
  }
  // Keep track of order
  std::unordered_map<int, std::vector<ILightWithId*>> lightsPreGroup;

  auto managers = UnityEngine::Object::FindObjectsOfType<TrackLaneRingsManager*>();

  for (auto* light : getLights()) {
    if (light == nullptr) {
      continue;
    }

    auto monoBehaviour = il2cpp_utils::try_cast<MonoBehaviour>(light);

    if (!monoBehaviour) {
      continue;
    }

    int z = static_cast<int>(std::round(static_cast<double>((*monoBehaviour)->get_transform()->get_position().z)));

    auto* ring = (*monoBehaviour)->GetComponentInParent<TrackLaneRing*>();

    if (ring != nullptr) {
      TrackLaneRingsManager* mngr = managers->FirstOrDefault([&](TrackLaneRingsManager* it) -> bool {
        return (it != nullptr) && std::find(it->_rings.begin(), it->_rings.end(), UnityW(ring)) != it->_rings.end();
      });

      if (mngr != nullptr) {
        z = 1000 + mngr->_rings->IndexOf(ring);
      }
    }

    auto& list = lightsPreGroup[z];
    list.push_back(light);
  }

  auto& lightMap = LightsPropagationGrouped.emplace();

  int i = 0;
  for (auto&& [z, list] : lightsPreGroup) {
    lightMap[i] = std::move(list);
    i++;
  }

  return lightMap;
}

void Chroma::LightColorizer::Colorize(bool refresh, std::optional<std::vector<GlobalNamespace::ILightWithId*>> const& selectLights,
                                      LightColorOptionalPalette const& colors) {
  for (int i = 0; i < colors.size(); i++) {
    _colors[i] = colors[i];
  }

  // Allow light colorizer to not force color
  if (!refresh) {
    return;
  }

  Refresh(selectLights);
}
Chroma::LightColorizer::LightColorPalette Chroma::LightColorizer::getColor() const {
  LightColorPalette colors;
  for (int i = 0; i < COLOR_FIELDS; i++) {
    std::optional<Sombrero::FastColor> color;

    auto colorIt = _colors.find(i);
    if (colorIt != _colors.end()) {
      color = colorIt->second;
    }

    if (!color) {
      color = GlobalColor[i];
    }

    if (!color) {
      color = _originalColors[i]->get_color();
    }

    colors[i] = *color;
  }

  return colors;
}
void Chroma::LightColorizer::Refresh(std::optional<std::vector<GlobalNamespace::ILightWithId*>> const& selectLights) const {
  _lightSwitchEventEffect->Refresh(false, selectLights);
}
