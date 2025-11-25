#include <algorithm>
#include <sstream>
#include <concepts>

#include "main.hpp"
#include "environment_enhancements/EnvironmentEnhancementManager.hpp"
#include "environment_enhancements/GeometryFactory.hpp"
#include "environment_enhancements/MaterialAnimator.hpp"
#include "environment_enhancements/LegacyEnvironmentRemoval.hpp"
#include "environment_enhancements/ComponentInitializer.hpp"
#include "environment_enhancements/ParametricBoxControllerParameters.hpp"
#include "hooks/LightWithIdManager.hpp"
#include "hooks/TrackLaneRingsManager.hpp"
#include "utils/ChromaUtils.hpp"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"

#include "GlobalNamespace/BeatmapObjectsAvoidance.hpp"
#include "GlobalNamespace/TrackLaneRing.hpp"

#include "tracks/shared/Animation/GameObjectTrackController.hpp"
#include "tracks/shared/AssociatedData.h"
#include "tracks/shared/Animation/PointDefinition.h"

#include "boost-regex/regex/include/boost/regex.hpp"

#include "paper2_scotland2/shared/Profiler.hpp"

using namespace Chroma;
using namespace ChromaUtils;
using namespace UnityEngine::SceneManagement;
using namespace Tracks;

using ChromaRegex = boost::regex;

// We can return a reference here since _globalGameObjectInfos is keeping the reference alive
std::vector<ByRef<GameObjectInfo const>> Chroma::EnvironmentEnhancementManager::LookupId(std::string_view const id,
                                                                                         Chroma::LookupMethod lookupMethod) {
  std::vector<ByRef<GameObjectInfo const>> ret;
  ret.reserve(_globalGameObjectInfos.size());

  auto doLookup = [&ret, &id](auto&& lookupMethodStr, auto&& predicate) constexpr {
    for (auto const& o : _globalGameObjectInfos) {
      // We have a try/catch here so the loop doesn't die
      try {
        if (predicate(o)) {
          ret.emplace_back(o);
        }
      } catch (std::exception& e) {
        ChromaLogger::Logger.error("Failed to match ({}) for lookup ({}) with id ({})", o.FullID.c_str(), lookupMethodStr, id.data());
        ChromaLogger::Logger.error("Error: {}", e.what());
      }
    }
  };

  // only set when needed
  ChromaRegex regex;

  std::string_view lookupMethodStr;

  try {
    switch (lookupMethod) {
    case LookupMethod::Regex: {
      lookupMethodStr = "Regex";
      regex = ChromaRegex(id.data(), boost::regex_constants::ECMAScript | boost::regex_constants::optimize);
      doLookup("Regex", [&regex](GameObjectInfo const& n) constexpr { return boost::regex_search(n.FullID, regex); });
      break;
    }

    case LookupMethod::Exact: {
      lookupMethodStr = "Exact";
      size_t idHash = std::hash<std::string_view>()(id);
      doLookup("Exact", [idHash](GameObjectInfo const& n) constexpr { return n.FullIDHash == idHash; });
      break;
    }

    case LookupMethod::Contains: {
      doLookup("Contains", [&id](GameObjectInfo const& n) constexpr { return n.FullID.find(id) != std::string::npos; });
      break;
    }

    case LookupMethod::StartsWith: {
      lookupMethodStr = "StartsWith";
      doLookup("StartsWith", [&id](GameObjectInfo const& n) constexpr { return n.FullID.starts_with(id); });
      break;
    }

    case LookupMethod::EndsWith: {
      lookupMethodStr = "EndsWith";

      doLookup("EndsWith", [&id](GameObjectInfo const& n) constexpr { return n.FullID.ends_with(id); });
      break;
    }

    default: {
      return {};
    }
    }
  } catch (std::exception const& e) {
    ChromaLogger::Logger.error("Failed to create match for lookup ({}) with id ({})", lookupMethodStr.data(), id.data());
    ChromaLogger::Logger.error("Error: {}", e.what());
  }

  ret.shrink_to_fit();

  return ret;
}

static std::optional<Sombrero::FastVector3> GetVectorData(rapidjson::Value const& dynData, std::string_view const name) {
  auto objectsValIt = dynData.FindMember(name.data());

  if (objectsValIt == dynData.MemberEnd()) {
    return std::nullopt;
  }

  auto const& objectsVal = objectsValIt->value;

  if (!objectsVal.IsArray() || objectsVal.Empty()) {
    return std::nullopt;
  }

  return Sombrero::FastVector3{ objectsVal[0].GetFloat(), objectsVal[1].GetFloat(), objectsVal[2].GetFloat() };
}

struct TransformData {
public:
  std::optional<Sombrero::FastVector3> scale;
  std::optional<Sombrero::FastVector3> position;
  std::optional<Sombrero::FastVector3> rotation;
  std::optional<Sombrero::FastVector3> localPosition;
  std::optional<Sombrero::FastVector3> localRotation;

  TransformData(TransformData&&) = default;

  TransformData(rapidjson::Value const& customData, bool v2, float noteLinesDistance) {
    scale = GetVectorData(customData, v2 ? NewConstants::V2_SCALE : NewConstants::SCALE);
    position = GetVectorData(customData, v2 ? NewConstants::V2_POSITION : NewConstants::POSITION);
    rotation = GetVectorData(customData, v2 ? NewConstants::V2_ROTATION : NewConstants::ROTATION);
    localPosition = GetVectorData(customData, v2 ? NewConstants::V2_LOCAL_POSITION : NewConstants::LOCAL_POSITION);
    localRotation = GetVectorData(customData, v2 ? NewConstants::V2_LOCAL_ROTATION : NewConstants::LOCAL_ROTATION);

    if (v2) {
      // ReSharper disable once UseNullPropagation
      if (position) {
        *position *= noteLinesDistance;
      }

      // ReSharper disable once UseNullPropagation
      if (localPosition) {
        *localPosition *= noteLinesDistance;
      }
    }
  }

  inline void Apply(UnityEngine::Transform* transform, bool leftHanded) const {
    Apply(transform, leftHanded, scale, position, rotation, localPosition, localRotation);
  }

private:
  static void Apply(UnityEngine::Transform* transform, bool leftHanded, std::optional<Sombrero::FastVector3> const& scale,
                    std::optional<Sombrero::FastVector3> const& position, std::optional<Sombrero::FastVector3> const& rotation,
                    std::optional<Sombrero::FastVector3> const& localPosition, std::optional<Sombrero::FastVector3> const& localRotation) {
    // TODO: Mirror
    //        if (leftHanded)
    //        {
    //            scale = scale?.Mirror();
    //            position = position?.Mirror();
    //            rotation = rotation?.Mirror();
    //            localPosition = localPosition?.Mirror();
    //            localRotation = localRotation?.Mirror();
    //        }

    if (scale) {
      transform->set_localScale(*scale);
    }

    if (localPosition) {
      transform->set_localPosition(*localPosition);
    } else if (position) {
      transform->set_position(*position);
    }

    if (localRotation) {
      transform->set_localEulerAngles(*localRotation);
    } else if (rotation) {
      transform->set_eulerAngles(*rotation);
    }
  }
};

void EnvironmentEnhancementManager::GetAllGameObjects() {
  LightIdRegisterer::canUnregister = true;

  _globalGameObjectInfos.clear();

  auto gameObjectsAll = UnityEngine::Resources::FindObjectsOfTypeAll<UnityEngine::GameObject*>();
  std::vector<UnityEngine::GameObject*> gameObjectsVec;
  gameObjectsVec.reserve(gameObjectsAll.size());

  // I'll probably revist this formula for getting objects by only grabbing the root objects and adding all the children
  for (int i = 0; i < gameObjectsAll.size(); i++) {
    auto* gameObject = gameObjectsAll.get(i);
    if (gameObject == nullptr) {
      continue;
    }

    auto sceneNameIl2cpp = gameObject->get_scene().get_name();
    if (sceneNameIl2cpp == nullptr) {
      continue;
    }

    std::u16string_view sceneName = sceneNameIl2cpp;

    if ((sceneName.find(u"Environment") != std::string::npos && sceneName.find(u"Menu") == std::string::npos) ||
        (gameObject->GetComponent<GlobalNamespace::TrackLaneRing*>() != nullptr)) {
      gameObjectsVec.emplace_back(gameObject);
    }
  }

  // Adds the children of whitelist GameObjects
  // Mainly for grabbing cone objects in KaleidoscopeEnvironment
  std::vector<UnityEngine::GameObject*> gameObjectsVec2 = { gameObjectsVec.begin(), gameObjectsVec.end() };
  for (auto& gameObject : gameObjectsVec) {
    std::vector<UnityEngine::Transform*> allChildren;
    GetChildRecursive(gameObject->get_transform(), allChildren);

    for (auto& transform : allChildren) {
      auto* childGameObject = transform->get_gameObject().ptr();
      if (std::ranges::find(gameObjectsVec, childGameObject) == gameObjectsVec.end()) {
        gameObjectsVec2.push_back(childGameObject);
      }
    }
  }

  for (auto const& gameObject : gameObjectsVec2) {
    if (gameObject == nullptr) {
      continue;
    }

    _globalGameObjectInfos.emplace_back(gameObject);
  }

  // Shrink if necessary
  _globalGameObjectInfos.shrink_to_fit();

  if (getChromaConfig().PrintEnvironmentEnhancementDebug.GetValue()) {
    // print objects
    std::thread([] {
      // copy
      auto objectsToPrint = std::vector<GameObjectInfo>(_globalGameObjectInfos);

      // Sort in order from shortest to longest string
      std::ranges::sort(objectsToPrint, [](GameObjectInfo const& a, GameObjectInfo const& b) { return a.FullID < b.FullID; });

      std::stringstream ss;
      for (auto const& o : objectsToPrint) {
        ss << o.FullID << '\n';
      }

      ChromaLogger::Logger.info("Objects found in environment:\n{}", ss.str().c_str());
    }).detach();
  }
}

void EnvironmentEnhancementManager::Init(CustomJSONData::CustomBeatmapData* customBeatmapData, Zenject::DiContainer* diContainer) {
  float noteLinesDistance = 0.6F;
  ChromaLogger::Logger.debug("Custom beat map {}", fmt::ptr(customBeatmapData));
  ChromaLogger::Logger.debug("Custom beat map custom data {}", fmt::ptr(customBeatmapData->customData));
  auto const& customDynWrapper = customBeatmapData->customData->value;
  bool v2 = customBeatmapData->v2orEarlier;
  CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Is environment v2 {}", v2);
  TracksAD::BeatmapAssociatedData& trackBeatmapAD = TracksAD::getBeatmapAD(customBeatmapData->customData);
  bool leftHanded = trackBeatmapAD.leftHanded;

  AvoidanceRotation.clear();
  AvoidancePosition.clear();
  RingRotationOffsets.clear();
  ParametricBoxControllerParameters::TransformParameters.clear();

  if (getChromaConfig().environmentEnhancementsEnabled.GetValue()) {
    // seriously what the fuck beat games
    // GradientBackground permanently yeeted because it looks awful and can ruin multi-colored chroma maps
    auto gradientBackground = UnityEngine::GameObject::Find("/Environment/GradientBackground");
    if (gradientBackground) {
      gradientBackground->SetActive(false);
    }
  }

  if (!customDynWrapper) {
    if (v2) {
      LegacyEnvironmentRemoval::Init(customBeatmapData);
    }
    return;
  }

  rapidjson::Value const& dynData = *customDynWrapper;

  auto environmentData = dynData.FindMember(v2 ? NewConstants::V2_ENVIRONMENT.data() : NewConstants::ENVIRONMENT.data());

  if (environmentData == dynData.MemberEnd()) {
    if (v2) {
      LegacyEnvironmentRemoval::Init(customBeatmapData);
    }
    return;
  }

  MaterialsManager materialsManager(dynData, trackBeatmapAD, v2);
  GeometryFactory geometryFactory(materialsManager, v2);

  GetAllGameObjects();
  auto rings = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::TrackLaneRingsManager*>();
  Chroma::TrackLaneRingsManagerHolder::RingManagers = { rings.begin(), rings.end() };

  std::vector<Paper::Profiler<std::chrono::milliseconds>> profileData;
  auto environmentDataObject = environmentData->value.GetArray();

  // Record start time
  auto startAll = std::chrono::high_resolution_clock::now();

  for (auto const& gameObjectDataVal : environmentDataObject) {
    // Record start time
    auto& profiler = profileData.emplace_back();
    profiler.startTimer();

    auto idMember = gameObjectDataVal.FindMember(v2 ? NewConstants::V2_GAMEOBJECT_ID.data() : NewConstants::GAMEOBJECT_ID.data());
    auto geometryMember = gameObjectDataVal.FindMember(v2 ? NewConstants::V2_GEOMETRY.data() : NewConstants::GEOMETRY.data());
    std::vector<ByRef<GameObjectInfo const>> foundObjects;

    if (idMember == gameObjectDataVal.MemberEnd() && geometryMember == gameObjectDataVal.MemberEnd()) {
      CJDLogger::Logger.fmtLog<Paper::LogLevel::ERR>("Missing geometry or lookup, skipping");
      continue;
    }
    if (idMember != gameObjectDataVal.MemberEnd() && geometryMember != gameObjectDataVal.MemberEnd()) {
      CJDLogger::Logger.fmtLog<Paper::LogLevel::ERR>("Can't use both lookup and geometry, skipping");
      continue;
    }

    if (idMember != gameObjectDataVal.MemberEnd()) {
      std::string_view id = idMember->value.GetString();
      std::string lookupString =
          gameObjectDataVal.FindMember(v2 ? NewConstants::V2_LOOKUP_METHOD.data() : NewConstants::LOOKUP_METHOD.data())->value.GetString();

      // Convert string to lower case
      std::ranges::transform(lookupString, lookupString.begin(), ::tolower);
      LookupMethod lookupMethod = LookupMethod::Exact;

      // Record JSON parse time
      profiler.mark("Parsing JSON for id " + std::string(id));

      if (lookupString == "regex") {
        lookupMethod = LookupMethod::Regex;
      } else if (lookupString == "exact") {
        lookupMethod = LookupMethod::Exact;
      } else if (lookupString == "contains") {
        lookupMethod = LookupMethod::Contains;
      } else if (lookupString == "startswith") {
        lookupMethod = LookupMethod::StartsWith;
      } else if (lookupString == "endswith") {
        lookupMethod = LookupMethod::EndsWith;
      }

      foundObjects = LookupId(id, lookupMethod);

      // Record find object time
      std::stringstream foundObjectsLog;
      foundObjectsLog << "Finding objects for id (" << std::to_string(foundObjects.size()) << ") " << id << " using " << lookupString;

      if (getChromaConfig().PrintEnvironmentEnhancementDebug.GetValue()) {
        ChromaLogger::Logger.info("ID [\"{}\"] using method [{}] found:", id.data(), lookupString.c_str());
      }

      profiler.mark(foundObjectsLog.str());
    }

    if (geometryMember != gameObjectDataVal.MemberEnd()) {
      auto goInfo = ByRef<GameObjectInfo const>(_globalGameObjectInfos.emplace_back(geometryFactory.Create(geometryMember->value)));
      // Record JSON parse time
      profiler.mark("Parsing JSON for geometry ");

      foundObjects.emplace_back(goInfo);
    }

    if (foundObjects.empty()) {
      profiler.mark("No objects found!", false);
      profiler.endTimer();
      continue;
    }

    if (getChromaConfig().PrintEnvironmentEnhancementDebug.GetValue()) {
      for (auto const& o : foundObjects) {
        ChromaLogger::Logger.info("{}", o.heldRef.FullID.c_str());
      }

      ChromaLogger::Logger.info("=====================================");
    }

    TransformData spawnData(gameObjectDataVal, v2, noteLinesDistance);

    std::optional<int> dupeAmount =
        getIfExists<int>(gameObjectDataVal, v2 ? NewConstants::V2_DUPLICATION_AMOUNT : NewConstants::DUPLICATION_AMOUNT);
    std::optional<bool> active = getIfExists<bool>(gameObjectDataVal, v2 ? NewConstants::V2_ACTIVE : NewConstants::ACTIVE);

    // Create track if objects are found
    auto trackNameIt = gameObjectDataVal.FindMember(v2 ? Chroma::NewConstants::V2_TRACK.data() : Chroma::NewConstants::TRACK.data());

    std::optional<std::string_view> trackName;
    std::vector<TrackW> tracks;

    if (trackNameIt != gameObjectDataVal.MemberEnd()) {
      auto const& trackJSON = trackNameIt->value;
      if (trackJSON.IsString()) {
        trackName = trackJSON.GetString();
        std::string_view val = *trackName;
        tracks = { trackBeatmapAD.getTrack(val) };
      } else if (trackJSON.IsArray()) {
        for (auto const& trackJSONItem : trackJSON.GetArray()) {
          trackName = trackJSON.GetString();
          std::string_view val = *trackName;
          tracks.emplace_back(trackBeatmapAD.getTrack(val));
        }
      }
    }

    std::vector<ByRef<GameObjectInfo const>> gameObjectInfos;
    if (dupeAmount) {
      gameObjectInfos.reserve(foundObjects.size() * dupeAmount.value());

      for (auto const& gameObjectInfoRef : foundObjects) {
        auto const& gameObjectInfo = gameObjectInfoRef.heldRef;
        if (getChromaConfig().PrintEnvironmentEnhancementDebug.GetValue()) {
          profiler.mark("Duplicating [" + gameObjectInfo.FullID + "]:", false);
        }

        auto* gameObject = gameObjectInfo.GameObject;
        auto parent = gameObject->get_transform()->get_parent();
        auto scene = gameObject->get_scene();

        for (int i = 0; i < dupeAmount.value(); i++) {
          std::vector<std::shared_ptr<IComponentData>> componentDatas;
          ComponentInitializer::PrefillComponentsData(gameObject->get_transform(), componentDatas);
          auto* newGameObject = UnityEngine::Object::Instantiate(gameObject);
          ComponentInitializer::PostfillComponentsData(newGameObject->get_transform(), gameObject->get_transform(), componentDatas);

          SceneManager::MoveGameObjectToScene(newGameObject, scene);
          newGameObject->get_transform()->SetParent(parent, true);

          auto const& newGameObjectInfo = ComponentInitializer::InitializeComponents(
              newGameObject->get_transform(), gameObject->get_transform(), _globalGameObjectInfos, componentDatas, diContainer);
          gameObjectInfos.emplace_back(newGameObjectInfo);
          // This is not needed as long as InitializeComponents adds to gameObjectInfos

          //                            for (auto const& o : _globalGameObjectInfos) {
          //                                if (o.GameObject->Equals(newGameObject)) {
          //                                    gameObjectInfos.emplace_back(o);
          //                                }
          //                            }
        }
      }
      // Record end time
      profiler.mark("Duping ");
    } else {

      // Better way of doing this?
      // For some reason, copy constructor is deleted?
      gameObjectInfos = std::move(foundObjects);
    }

    for (auto const& gameObjectInfoRef : gameObjectInfos) {
      auto const& gameObjectInfo = gameObjectInfoRef.heldRef;

      auto* gameObject = gameObjectInfo.GameObject;

      if (active) {
        gameObject->SetActive(active.value());
      }

      auto transform = gameObject->get_transform();

      spawnData.Apply(transform, leftHanded);
      auto const& position = spawnData.position;
      auto const& localPosition = spawnData.localPosition;
      auto const& rotation = spawnData.rotation;
      auto const& localRotation = spawnData.localRotation;
      auto const& scale = spawnData.scale;

      if (position) {
        CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>(
            "Setting position to go {} to {} {} {}", gameObject->get_name().operator std::string(), position->x, position->y, position->z);
      }

      // Handle TrackLaneRing
      auto* trackLaneRing = gameObject->GetComponentInChildren<GlobalNamespace::TrackLaneRing*>();
      if (trackLaneRing != nullptr) {
        if (position || localPosition) {
          trackLaneRing->_positionOffset = trackLaneRing->get_transform()->get_localPosition();
          trackLaneRing->_posZ = 0;
        }

        if (rotation || localRotation) {
          RingRotationOffsets[trackLaneRing] = trackLaneRing->get_transform()->get_localRotation();
          trackLaneRing->_rotZ = 0;
        }
      }

      // Handle ParametricBoxController
      auto* parametricBoxController = gameObject->GetComponentInChildren<GlobalNamespace::ParametricBoxController*>();
      if (parametricBoxController != nullptr) {
        if (position || localPosition) {
          ParametricBoxControllerParameters::SetTransformPosition(parametricBoxController,
                                                                  parametricBoxController->get_transform()->get_localPosition());
        }

        if (scale) {
          ParametricBoxControllerParameters::SetTransformScale(parametricBoxController,
                                                               parametricBoxController->get_transform()->get_localScale());
        }
      }

      // apparently not needed for BS 1.37+?
      // auto* beatmapObjectsAvoidance = gameObject->GetComponentInChildren<GlobalNamespace::BeatmapObjectsAvoidance*>();
      // if (beatmapObjectsAvoidance != nullptr) {
      //   if (position || localPosition) {
      //     AvoidancePosition[beatmapObjectsAvoidance] = beatmapObjectsAvoidance->get_transform()->get_localPosition();
      //   }
      //   if (rotation || localRotation) {
      //     AvoidanceRotation[beatmapObjectsAvoidance] = beatmapObjectsAvoidance->get_transform()->get_localRotation();
      //   }
      // }

      ComponentInitializer::InitializeCustomComponents(gameObject, gameObjectDataVal, v2);

      GameObjectTrackController* controller = nullptr;

      if (!tracks.empty()) {
        controller = GameObjectTrackController::HandleTrackData(gameObject, tracks, noteLinesDistance, v2, true).value_or(nullptr);
      }

      if (controller == nullptr) {
        continue;
      }
      auto& controllerData = controller->getTrackControllerData();

      if (trackLaneRing != nullptr) {
        controllerData.RotationUpdate += [=]() { RingRotationOffsets[trackLaneRing] = trackLaneRing->transform->get_localRotation(); };
        controllerData.PositionUpdate += [=]() { trackLaneRing->_positionOffset = trackLaneRing->transform->get_localPosition(); };
      }else if (parametricBoxController != nullptr) {
        auto* parametricBoxControllerTransform = parametricBoxController->get_transform().ptr();
        controllerData.ScaleUpdate += [=]() {
          ParametricBoxControllerParameters::SetTransformScale(parametricBoxController, parametricBoxControllerTransform->get_localScale());
        };
        controllerData.PositionUpdate += [=]() {
          ParametricBoxControllerParameters::SetTransformPosition(parametricBoxController,
                                                                  parametricBoxControllerTransform->get_localPosition());
        };
      }

      for (const auto& track : tracks) {
        track.RegisterGameObject(gameObject);
      }

      // BS 1.37+ apparently doesn't need this
      // if (beatmapObjectsAvoidance != nullptr) {
      //   controllerData.RotationUpdate += [=]() {
      //     AvoidanceRotation[beatmapObjectsAvoidance] = beatmapObjectsAvoidance->transform->get_localRotation();
      //   };
      //   controllerData.PositionUpdate += [=]() {
      //     AvoidancePosition[beatmapObjectsAvoidance] = beatmapObjectsAvoidance->transform->get_localPosition();
      //   };
      // }
    }

    // Record end time
    profiler.endTimer();
  }

  // Record all end time
  auto finish = std::chrono::high_resolution_clock::now();
  auto millisElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - startAll).count();

  std::thread([profileData = std::move(profileData), millisElapsed] {
    // Log all objects
    for (auto const& profile : profileData) {
      profile.printMarks();
      ChromaLogger::Logger.info("=====================================\n");
    }

    ChromaLogger::Logger.info("Finished environment enhancements took {}ldms", millisElapsed);
  }).detach();

  ///  Handle materials
  auto const& materials = materialsManager.GetMaterials();
  std::vector<MaterialInfo> animatedMaterials;
  animatedMaterials.reserve(materials.size());

  for (auto const& [s, m] : materials) {
    if (!m.Track || m.Track->empty()) {
      continue;
    }
    animatedMaterials.emplace_back(m);
  }

  if (!animatedMaterials.empty()) {
    auto* animated = UnityEngine::GameObject::New_ctor("MaterialAnimator")->AddComponent<MaterialAnimator*>();
    // animated->context = TracksAD::getBeatmapAD(customBeatmapData->customData).internal_tracks_context;
    animated->materials = std::move(animatedMaterials);
  }
}

void EnvironmentEnhancementManager::GetChildRecursive(UnityEngine::Transform* gameObject, std::vector<UnityEngine::Transform*>& children) {
  children.reserve(children.size() + gameObject->get_childCount());
  auto gameObjectChildCount = gameObject->get_childCount();
  for (int i = 0; i < gameObjectChildCount; i++) {
    auto* child = gameObject->GetChild(i).ptr();
    children.push_back(child);
    GetChildRecursive(child, children);
  }
}
