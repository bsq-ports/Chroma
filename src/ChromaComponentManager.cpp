#include "ChromaComponentManager.hpp"
#include "Chroma.hpp"
#include "lighting/ChromaFogController.hpp"

#include "tracks/shared/Animation/Track.h"
#include "tracks/shared/Animation/Easings.h"
#include "tracks/shared/Animation/PointDefinition.h"
#include "tracks/shared/StaticHolders.hpp"
#include "tracks/shared/bindings.h" // Correct include for Functions enum
#include "custom-json-data/shared/CustomBeatmapData.h"

#include "UnityEngine/Component.hpp"
#include "GlobalNamespace/BeatmapCallbacksController.hpp"
#include "GlobalNamespace/BloomFogSO.hpp"
#include "GlobalNamespace/BloomFogEnvironment.hpp"
#include "GlobalNamespace/BloomFogEnvironmentParams.hpp"
#include "GlobalNamespace/TubeBloomPrePassLight.hpp"

#include <unordered_map>
#include <span>
#include <string_view>

struct CoroutineInfo {
  CoroutineInfo(std::string_view componentName, std::vector<UnityEngine::Component*> component, PointDefinitionW const points,
                float duration, float startTime, Functions easing)
      : componentName(componentName), component(std::move(component)), points(points), duration(duration), startTime(startTime),
        easing(easing) {}

  CoroutineInfo(CoroutineInfo const&) = delete;
  ~CoroutineInfo() = default;
  CoroutineInfo(CoroutineInfo&&) = default;

  std::string_view componentName;
  std::vector<UnityEngine::Component*> component;
  PointDefinitionW points;
  float duration;
  float startTime;
  Functions easing;
};

// PROPERTY NAME -> TRACKFORGAMEOBJECTS & CORO
static std::unordered_map<std::string, std::unordered_map<TrackW, CoroutineInfo>> coroutines;

void animateBloomFog(std::string_view propName, std::span<UnityEngine::Component* const> components, float val) {
  if (components.empty()) {
    return;
  }

  auto handleFog = [&](auto&& func) constexpr {
    for (auto const& c : components) {
      func(reinterpret_cast<GlobalNamespace::BloomFogEnvironment*>(c));
    }
  };
  if (propName == Chroma::NewConstants::ATTENUATION) {
    handleFog([&](GlobalNamespace::BloomFogEnvironment* fog) { fog->fogParams->attenuation = Chroma::fogAttenuationFix(val); });
  }
  if (propName == Chroma::NewConstants::OFFSET) {
    handleFog([&](GlobalNamespace::BloomFogEnvironment* fog) { fog->fogParams->offset = val; });
  }
  if (propName == Chroma::NewConstants::HEIGHT_FOG_STARTY) {
    handleFog([&](GlobalNamespace::BloomFogEnvironment* fog) { fog->fogParams->heightFogStartY = val; });
  }
  if (propName == Chroma::NewConstants::HEIGHT_FOG_HEIGHT) {
    handleFog([&](GlobalNamespace::BloomFogEnvironment* fog) { fog->fogParams->heightFogHeight = val; });
  }
}
void animateTubeBloom(std::string_view propName, std::span<UnityEngine::Component* const> components, float val) {
  if (components.empty()) {
    return;
  }

  auto handleTube = [&](auto&& func) {
    for (auto const& c : components) {
      func(reinterpret_cast<GlobalNamespace::TubeBloomPrePassLight*>(c));
    }
  };
  if (propName == Chroma::NewConstants::COLOR_ALPHA_MULTIPLIER) {
    handleTube([&](GlobalNamespace::TubeBloomPrePassLight* tube) {
      tube->_colorAlphaMultiplier = val;
      tube->MarkDirty();
    });
  }
  if (propName == Chroma::NewConstants::BLOOM_FOG_INTENSITY_MULTIPLIER) {
    handleTube([&](GlobalNamespace::TubeBloomPrePassLight* tube) { tube->bloomFogIntensityMultiplier = val; });
  }
}

///
/// \tparam skipToLast
/// \param context
/// \param songTime
/// \return true if not finished. If false, this coroutine is finished
template <bool skipToLast = false> static bool UpdateCoroutine(std::string_view propName, CoroutineInfo const& context, float songTime) {
  float elapsedTime = songTime - context.startTime;

  // Wait, the coroutine is too early
  if (elapsedTime < 0) {
    return true;
  }

  float normalizedTime = context.duration > 0 ? std::min(elapsedTime / context.duration, 1.0F) : 1;
  float time = Easings::Interpolate(normalizedTime, context.easing);

  bool last = false;

  float val = context.points.InterpolateLinear(time, last);

  bool finished = last || context.duration <= 0;

  if (context.componentName == Chroma::NewConstants::BLOOM_FOG_ENVIRONMENT) {
    animateBloomFog(propName, std::span(context.component), val);
  }

  if (context.componentName == Chroma::NewConstants::TUBE_BLOOM_PRE_PASS_LIGHT) {
    animateTubeBloom(propName, context.component, val);
  }

  // continue only if not finished or if elapsedTime is less than duration
  return !finished && elapsedTime < context.duration;
}

void Chroma::Component::UpdateCoroutines(GlobalNamespace::BeatmapCallbacksController* callbackController) {
  auto songTime = callbackController->songTime;
  for (auto& coroutineGroup : coroutines) {

    auto const& componentName = coroutineGroup.first;
    auto& props = coroutineGroup.second;

    // No coros
    if (props.empty()) {
      continue;
    }

    for (auto it = props.begin(); it != props.end();) {
      if (UpdateCoroutine(componentName, it->second, songTime)) {
        it++;
      } else {
        it = props.erase(it);
      }
    }
  }
}
static std::vector<UnityEngine::Component*> getComponentsFromType(std::string_view componentName, TrackW track) {
  std::vector<UnityEngine::Component*> components;
  auto gameObjects = track.GetGameObjects();
  components.reserve(gameObjects.size());

  for (auto const& go : gameObjects) {
    // fog
    if (componentName == Chroma::NewConstants::BLOOM_FOG_ENVIRONMENT) {
      auto fogs = go->GetComponentsInChildren<GlobalNamespace::BloomFogEnvironment*>();
      for (auto const& fog : fogs) {
        components.push_back(fog);
      }
    }
    // tube
    if (componentName == Chroma::NewConstants::TUBE_BLOOM_PRE_PASS_LIGHT) {
      auto fogs = go->GetComponentsInChildren<GlobalNamespace::TubeBloomPrePassLight*>();
      for (auto const& tube : fogs) {
        components.push_back(tube);
      }
    }
  }
  return components;
}

void Chroma::Component::StartEvent(GlobalNamespace::BeatmapCallbacksController* callbackController,
                                   CustomJSONData::CustomEventData* customEventData,
                                   ChromaEvents::AnimateComponentEventData const& eventAD ) {

  auto beatmap = callbackController->_beatmapData;
  auto customBeatmap = il2cpp_utils::cast<CustomJSONData::CustomBeatmapData>(beatmap);
  auto& beatmapAD = TracksAD::getBeatmapAD(customBeatmap->customData);

  auto duration = eventAD.duration;

  auto bpm = TracksStatic::bpmController->currentBpm; // spawnController->get_currentBpm()

  duration = 60.0F * duration / bpm;

  auto easing = eventAD.easing;

  bool noDuration = duration == 0 || customEventData->time + (duration * 1) < callbackController->songTime;

  for (auto const& [componentName, props] : eventAD.coroutineInfos) {
    for (auto const& track : eventAD.track) {
      std::vector<UnityEngine::Component*> components = getComponentsFromType(componentName, track);

      if (components.empty()) {
        continue;
      }

      for (auto const& [propName, prop] : props) {
        auto& propCoros = coroutines[std::string(propName)];
        propCoros.emplace(track, CoroutineInfo(componentName, components, prop, duration, customEventData->time, easing));
      }
    }
  }
}
