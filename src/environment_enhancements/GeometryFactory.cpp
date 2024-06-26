#include "environment_enhancements/GeometryFactory.hpp"
#include "environment_enhancements/ParametricBoxControllerParameters.hpp"
#include "utils/ChromaUtils.hpp"

#include "UnityEngine/PrimitiveType.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/MeshCollider.hpp"
#include "UnityEngine/MeshFilter.hpp"
#include "UnityEngine/Mesh.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Rendering/ShadowCastingMode.hpp"

#include "GlobalNamespace/TubeBloomPrePassLight.hpp"
#include "GlobalNamespace/ParametricBoxController.hpp"
#include "GlobalNamespace/SaberModelContainer.hpp"
#include "GlobalNamespace/TubeBloomPrePassLightWithId.hpp"
#include "Zenject/DiContainer.hpp"
#include "hooks/LightWithIdManager.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;

GeometryType geometryTypeFromString(std::string_view str) {
  if (str == "Cube") {
    return GeometryType::Cube;
  }
  if (str == "Sphere") {
    return GeometryType::Sphere;
  }
  if (str == "Capsule") {
    return GeometryType::Capsule;
  }
  if (str == "Cylinder") {
    return GeometryType::Cylinder;
  }
  if (str == "Plane") {
    return GeometryType::Plane;
  }
  if (str == "Quad") {
    return GeometryType::Quad;
  }
  if (str == "Triangle") {
    return GeometryType::Triangle;
  }

  ChromaLogger::Logger.error("Unknown geometry type {}", str.data());
  return Chroma::GeometryType::Cube;
}

GeometryFactory::GeometryFactory(MaterialsManager& materialsManager, bool v2)
    : materialsManager(materialsManager), v2(v2) {
  auto* tube = CRASH_UNLESS(Resources::FindObjectsOfTypeAll<TubeBloomPrePassLight*>())->FirstOrDefault();

  if (tube != nullptr) {
    _originalTubeBloomPrePassLight = tube;
  }
  lightWithIDManager = Resources::FindObjectsOfTypeAll<SaberModelContainer*>()->FirstOrDefault()->_container->Resolve<LightWithIdManager*>();
}

GameObject* Chroma::GeometryFactory::Create(rapidjson::Value const& data) {
  GeometryType geometryType;
  auto geometryStr = ChromaUtils::getIfExists<std::string_view>(data, v2 ? NewConstants::V2_GEOMETRY_TYPE
                                                                         : NewConstants::GEOMETRY_TYPE);
  bool collision =
      ChromaUtils::getIfExists<bool>(data, v2 ? NewConstants::V2_COLLISION : NewConstants::COLLISION).value_or(false);

  if (!geometryStr) {
    geometryType = GeometryType::Cube;
  } else {
    geometryType = geometryTypeFromString(geometryStr->data());
  }

  MaterialInfo materialInfo =
      materialsManager
          .GetMaterial(data.FindMember(v2 ? NewConstants::V2_MATERIAL.data() : NewConstants::MATERIAL.data())->value)
          .value();

  ShaderType shaderType = materialInfo.ShaderType;

  UnityEngine::PrimitiveType primitiveType;
  switch (geometryType) {
  case GeometryType::Sphere:
    primitiveType = UnityEngine::PrimitiveType::Sphere;
    break;
  case GeometryType::Capsule:
    primitiveType = UnityEngine::PrimitiveType::Capsule;
    break;
  case GeometryType::Cylinder:
    primitiveType = UnityEngine::PrimitiveType::Cylinder;
    break;
  case GeometryType::Cube:
    primitiveType = UnityEngine::PrimitiveType::Cube;
    break;
  case GeometryType::Plane:
    primitiveType = UnityEngine::PrimitiveType::Plane;
    break;
  case GeometryType::Quad:
  case GeometryType::Triangle:
    primitiveType = UnityEngine::PrimitiveType::Quad;
    break;
  }

  UnityEngine::GameObject* go = UnityEngine::GameObject::CreatePrimitive(primitiveType);
  go->set_name(std::string(geometryStr.value_or("")) + materialInfo.ShaderTypeStr);
  go->set_layer(14);

  auto* meshRenderer = go->GetComponent<MeshRenderer*>();

  // Disable expensive shadows
  meshRenderer->set_shadowCastingMode(Rendering::ShadowCastingMode::Off);
  meshRenderer->set_receiveShadows(false);

  // Shared material is usually better performance as far as I know
  auto* material = (Material*)materialInfo.Material;
  meshRenderer->set_sharedMaterial(material);

  if (geometryType == GeometryType::Triangle) {
    Mesh* mesh = ChromaUtils::CreateTriangleMesh();
    go->GetComponent<MeshFilter*>()->set_sharedMesh(mesh);
    if (collision) {
      auto* meshCollider = go->GetComponent<MeshCollider*>();
      if (meshCollider != nullptr) {
        meshCollider->set_sharedMesh(mesh);
      }
    }
  }

  if (!collision) {
    // destroy colliders
    Object::Destroy(go->GetComponent<Collider*>());
  }

  // Handle light preset
  if (!IsLightType(shaderType)) {
    return go;
  }

  // Stop TubeBloomPrePassLight from running OnEnable before I can set the fields
  go->SetActive(false);

  auto* tubeBloomPrePassLight = go->AddComponent<TubeBloomPrePassLight*>();
  auto* parametricBoxController = go->AddComponent<ParametricBoxController*>();

  ParametricBoxControllerParameters::SetTransformPosition(
      parametricBoxController, parametricBoxController->get_transform()->get_localPosition());
  ParametricBoxControllerParameters::SetTransformScale(parametricBoxController,
                                                       parametricBoxController->get_transform()->get_localScale());
  parametricBoxController->_meshRenderer = meshRenderer;

  if (_originalTubeBloomPrePassLight) {
    auto* origTube = *_originalTubeBloomPrePassLight;

    tubeBloomPrePassLight->_mainEffectPostProcessEnabled = origTube->_mainEffectPostProcessEnabled;
    tubeBloomPrePassLight->_lightType = origTube->_lightType;
    tubeBloomPrePassLight->_registeredWithLightType = origTube->_registeredWithLightType;
  } else {
    CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("[{{nameof(_originalTubeBloomPrePassLight)}}] was null.");
    throw std::runtime_error("[{nameof(_originalTubeBloomPrePassLight)}] was null.");
  }

  tubeBloomPrePassLight->_parametricBoxController = parametricBoxController;

  auto* tubeBloomPrePassLightWithId = go->AddComponent<TubeBloomPrePassLightWithId*>();
  tubeBloomPrePassLightWithId->_lightManager = lightWithIDManager.ptr();
  tubeBloomPrePassLightWithId->_tubeBloomPrePassLight = tubeBloomPrePassLight;

  LightIdRegisterer::MarkForTableRegister(tubeBloomPrePassLightWithId->i___GlobalNamespace__ILightWithId());

  go->SetActive(true);

  return go;
}

bool Chroma::IsLightType(ShaderType shaderType) {
  return shaderType == ShaderType::OpaqueLight || shaderType == ShaderType::TransparentLight ||
         shaderType == ShaderType::BillieWater;
}
