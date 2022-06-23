#include "environment_enhancements/GeometryFactory.hpp"
#include "environment_enhancements/ParametricBoxControllerParameters.hpp"
#include "utils/ChromaUtils.hpp"

#include "UnityEngine/PrimitiveType.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/MeshCollider.hpp"
#include "UnityEngine/MeshFilter.hpp"
#include "UnityEngine/Mesh.hpp"
#include "UnityEngine/Rendering/ShadowCastingMode.hpp"
#include "UnityEngine/MaterialGlobalIlluminationFlags.hpp"

#include "GlobalNamespace/TubeBloomPrePassLight.hpp"
#include "GlobalNamespace/ParametricBoxController.hpp"
#include "GlobalNamespace/SaberModelContainer.hpp"
#include "GlobalNamespace/TubeBloomPrePassLightWithId.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;

using CacheKey = std::tuple<Sombrero::FastColor, ShaderType, std::optional<std::vector<std::string>>>;

namespace std {
    template <>
    struct hash<CacheKey>
    {
        size_t operator()(const CacheKey& key) const
        {
            std::hash<Sombrero::FastColor> c;
            std::hash<int> i;
            std::hash<std::string_view> s;

            auto hash = c(get<0>(key)) ^ i((int) get<1>(key));

            auto strs = get<2>(key);
            if (strs) {
                for (auto const &str: *strs) {
                    hash = hash ^ s(str);
                }
            }

            return hash;
        }
    };
}


inline static std::unordered_map<CacheKey, SafePtr<UnityEngine::Material>> _cachedMaterials;

GeometryType geometryTypeFromString(auto&& str) {
    if (str == "Sphere") {
        return GeometryType::Sphere;
    } else if (str == "Capsule") {
        return GeometryType::Capsule;
    } else if (str == "Cylinder") {
        return GeometryType::Cylinder;
    } else if (str == "Cube") {
        return GeometryType::Cube;
    } else if (str == "Plane") {
        return GeometryType::Plane;
    } else if (str == "Quad") {
        return GeometryType::Quad;
    } else if (str == "Triangle") {
        return GeometryType::Triangle;
    }

    getLogger().error("Unknown geometry type %s", str);
    return Chroma::GeometryType::Cube;
}

ShaderType shaderTypeFromString(auto&& str) {
    if (str == "Standard") {
        return ShaderType::Standard;
    } else if (str == "OpaqueLight") {
        return ShaderType::OpaqueLight;
    } else if (str == "TransparentLight") {
        return ShaderType::TransparentLight;
    }

    getLogger().error("Unknown shader type %s", str);
    return Chroma::ShaderType::Standard;
}

void GeometryFactory::reset() {
    auto tube = Resources::FindObjectsOfTypeAll<TubeBloomPrePassLight*>().FirstOrDefault();

    if (tube) _originalTubeBloomPrePassLight = tube;


    instantiator = (Zenject::IInstantiator*) Resources::FindObjectsOfTypeAll<SaberModelContainer*>().FirstOrDefault()->container;
}

GameObject * Chroma::GeometryFactory::Create(rapidjson::Value const &data) {
    auto color = ChromaUtils::ChromaUtilities::GetColorFromData(data).value_or(Sombrero::FastColor(0,0,0,0));
    GeometryType geometryType;
    auto geometryStr = ChromaUtils::getIfExists<std::string_view>(data, NewConstants::GEOMETRY_TYPE);
    auto shaderTypeStr = ChromaUtils::getIfExists<std::string_view>(data, NewConstants::SHADER_PRESET);
    ShaderType shaderType = shaderTypeStr ? shaderTypeFromString(shaderTypeStr->data()) : ShaderType::Standard;
    bool collision = ChromaUtils::getIfExists<bool>(data, NewConstants::COLLISION).value_or(false);

    std::optional<std::vector<std::string>> shaderKeywords;

    if (!geometryStr) geometryType = GeometryType::Cube;
    else geometryType = geometryTypeFromString(geometryStr->data());

    auto shaderKeywordsIt = data.FindMember(NewConstants::SHADER_KEYWORDS.data());
    if (shaderKeywordsIt != data.MemberEnd()) {
        shaderKeywords.emplace();
        auto arr = shaderKeywordsIt->value.GetArray();
        shaderKeywords->reserve(arr.Size());
        for (auto const& o : arr) {
            shaderKeywords->emplace_back(o.GetString());
        }
    }

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
            primitiveType = UnityEngine::PrimitiveType::Quad;
            break;
        case GeometryType::Triangle:
            primitiveType = UnityEngine::PrimitiveType::Quad;
            break;
    }

    UnityEngine::GameObject* go = UnityEngine::GameObject::CreatePrimitive(primitiveType);
    go->set_name(geometryStr.value_or("") + shaderTypeStr.value_or(""));

    auto* meshRenderer = go->GetComponent<MeshRenderer*>();

    // Disable expensive shadows
    meshRenderer->set_shadowCastingMode(Rendering::ShadowCastingMode::Off);
    meshRenderer->set_receiveShadows(false);

    // Shared material is usually better performance as far as I know
    Material* material = GetMaterial(color, shaderType, shaderKeywords);
    meshRenderer->set_sharedMaterial(material);

    if (geometryType == GeometryType::Triangle)
    {
        Mesh* mesh = ChromaUtils::CreateTriangleMesh();
        go->GetComponent<MeshFilter*>()->set_sharedMesh(mesh);
        if (collision)
        {
            auto* meshCollider = go->GetComponent<MeshCollider*>();
            if (meshCollider)
            {
                meshCollider->set_sharedMesh(mesh);
            }
        }
    }

    if (!collision)
    {
        // destroy colliders
        Object::Destroy(go->GetComponent<Collider*>());
    }

    // Handle light preset
    if (!IsLightType(shaderType))
    {
        return go;
    }

    // Stop TubeBloomPrePassLight from running OnEnable before I can set the fields
    go->SetActive(false);

    auto* tubeBloomPrePassLight = go->AddComponent<TubeBloomPrePassLight*>();
    auto* parametricBoxController = go->AddComponent<ParametricBoxController*>();

    ParametricBoxControllerParameters::SetTransformPosition(parametricBoxController, parametricBoxController->get_transform()->get_localPosition());
    ParametricBoxControllerParameters::SetTransformScale(parametricBoxController, parametricBoxController->get_transform()->get_localScale());
    parametricBoxController->meshRenderer = meshRenderer;


    if (_originalTubeBloomPrePassLight)
    {
        auto origTube = *_originalTubeBloomPrePassLight;

        tubeBloomPrePassLight->mainEffectPostProcessEnabled = origTube->mainEffectPostProcessEnabled;
        tubeBloomPrePassLight->lightType = origTube->lightType;
        tubeBloomPrePassLight->registeredWithLightType = origTube->registeredWithLightType;
    }
    else
    {
        throw std::runtime_error("[{nameof(_originalTubeBloomPrePassLight)}] was null.");
    }

    tubeBloomPrePassLight->parametricBoxController = parametricBoxController;

    auto tubeBloomPrePassLightWithId = instantiator->InstantiateComponent<TubeBloomPrePassLightWithId*>(go);
    tubeBloomPrePassLightWithId->tubeBloomPrePassLight = tubeBloomPrePassLight;

    go->SetActive(true);

    return go;

}

UnityEngine::Material *GeometryFactory::InstantiateSharedMaterial(ShaderType shaderType) {
    static ConstString opaqueLight("Custom/OpaqueNeonLight");
    static ConstString transparentLight("Custom/TransparentNeonLight");
    static ConstString standardBTSCube("Custom/SimpleLit");

    StringW shaderName;
    ArrayW<StringW> shaderKeywords;



    MaterialGlobalIlluminationFlags globalIlluminationFlags = IsLightType(shaderType) ? MaterialGlobalIlluminationFlags::EmissiveIsBlack
                                                                                      : MaterialGlobalIlluminationFlags::RealtimeEmissive;


    // Keywords found in RUE PC in BS 1.23
    switch (shaderType) {
        default:
            shaderName = standardBTSCube;
            shaderKeywords = ArrayW<StringW>(std::initializer_list<StringW>({
                                                                                    "DIFFUSE", "ENABLE_DIFFUSE",
                                                                                    "ENABLE_FOG", "ENABLE_HEIGHT_FOG",
                                                                                    "ENABLE_SPECULAR", "FOG",
                                                                                    "HEIGHT_FOG",
                                                                                    "REFLECTION_PROBE_BOX_PROJECTION",
                                                                                    "SPECULAR",
                                                                                    "_EMISSION",
                                                                                    "_ENABLE_FOG_TINT",
                                                                                    "_RIMLIGHT_NONE", "_ZWRITE_ON",
                                                                                    "REFLECTION_PROBE", "LIGHT_FALLOFF"
                                                                            }));
            break;
        case ShaderType::OpaqueLight:
            shaderName = opaqueLight;
            shaderKeywords = ArrayW<StringW>(std::initializer_list<StringW>({
                                                                                    "DIFFUSE", "ENABLE_BLUE_NOISE",
                                                                                    "ENABLE_DIFFUSE",
                                                                                    "ENABLE_HEIGHT_FOG",
                                                                                    "ENABLE_LIGHTNING", "USE_COLOR_FOG"
                                                                            }));
            break;
        case ShaderType::TransparentLight:
            shaderName = transparentLight;
            shaderKeywords = ArrayW<StringW>(std::initializer_list<StringW>({
                                                                                    "ENABLE_HEIGHT_FOG",
                                                                                    "MULTIPLY_COLOR_WITH_ALPHA",
                                                                                    "_ENABLE_MAIN_EFFECT_WHITE_BOOST"
                                                                            }));
            break;
    }

    auto shader = Shader::Find(shaderName);
    auto material = Material::New_ctor(shader);

    material->set_globalIlluminationFlags(globalIlluminationFlags);
    material->set_enableInstancing(true);

    if (shaderKeywords) {
        material->set_shaderKeywords(shaderKeywords);
    }

    return material;
}

UnityEngine::Material *GeometryFactory::GetMaterial(Sombrero::FastColor const &color, ShaderType shaderType,
                                                    std::optional<std::vector<std::string>> const &keywords) {
    auto& material = _cachedMaterials[std::make_tuple(color, shaderType, keywords)];


    if (material) return (Material *) material;

    static SafePtr<Material> _standardMaterial = InstantiateSharedMaterial(ShaderType::Standard);
    static SafePtr<Material> _opaqueLightMaterial = InstantiateSharedMaterial(ShaderType::OpaqueLight);
    static SafePtr<Material> _transparentLightMaterial = InstantiateSharedMaterial(ShaderType::TransparentLight);


    Material* originalMaterial;
    switch (shaderType) {
        default:
            originalMaterial = (Material *) _standardMaterial;
            break;
        case ShaderType::OpaqueLight:
            originalMaterial = (Material *) _opaqueLightMaterial;
            break;
        case ShaderType::TransparentLight:
            originalMaterial = (Material *) _transparentLightMaterial;
            break;
    }


    material = Object::Instantiate(originalMaterial);
    material->set_color(color);
    if (keywords)
    {
        auto sKeywords = ArrayW<StringW>(keywords->size());
        for (int i = 0; i < keywords->size(); i++) {
            sKeywords[i] = (*keywords)[i];
        }
        material->set_shaderKeywords(sKeywords);
    }

    return (Material *) material;
}


bool GeometryFactory::IsLightType(ShaderType shaderType) {
    return shaderType == ShaderType::OpaqueLight || shaderType == ShaderType::TransparentLight;;
}


