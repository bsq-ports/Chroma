#pragma once

#include <vector>

#include "environment_enhancements/GameObjectInfo.hpp"

#include "sombrero/shared/Vector3Utils.hpp"
#include "sombrero/shared/QuaternionUtils.hpp"

namespace UnityEngine {
class Transform;
}
namespace GlobalNamespace {
class BeatmapObjectData;
class BeatmapObjectSpawnController;
class BeatmapCallbacksController;
class BloomFogEnvironment;
class TubeBloomPrePassLight;
class BeatmapObjectsAvoidance;
class TrackLaneRing;
} // namespace GlobalNamespace

namespace CustomJSONData {
class CustomBeatmapData;
}

namespace Zenject {
class DiContainer;
}

namespace Chroma {
enum class LookupMethod { Regex, Exact, Contains, StartsWith, EndsWith };

class EnvironmentEnhancementManager {
private:
  inline static std::vector<GameObjectInfo> _globalGameObjectInfos;

  static std::vector<ByRef<GameObjectInfo const>> LookupId(std::string_view id, LookupMethod lookupMethod);

  static void GetAllGameObjects();

  static void GetChildRecursive(UnityEngine::Transform* gameObject, std::vector<UnityEngine::Transform*>& children);

public:
  inline static std::unordered_map<GlobalNamespace::TrackLaneRing*, Sombrero::FastQuaternion> RingRotationOffsets;
  inline static std::unordered_map<GlobalNamespace::BeatmapObjectsAvoidance*, Sombrero::FastVector3> AvoidancePosition;
  inline static std::unordered_map<GlobalNamespace::BeatmapObjectsAvoidance*, Sombrero::FastQuaternion>
      AvoidanceRotation;

  static void Init(CustomJSONData::CustomBeatmapData* customBeatmapData, Zenject::DiContainer* diContainer);
};
} // namespace Chroma
