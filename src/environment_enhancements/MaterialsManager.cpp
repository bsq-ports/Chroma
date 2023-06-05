#include "environment_enhancements/MaterialsManager.hpp"
#include "Chroma.hpp"
#include "utils/ChromaUtils.hpp"
#include "UnityEngine/MaterialGlobalIlluminationFlags.hpp"
#include "environment_enhancements/EnvironmentMaterialManager.hpp"

using namespace Chroma;
using namespace UnityEngine;

ShaderType shaderTypeFromString(std::string_view str) {
#define READ_ENUM(m) \
    if (str == #m) return ShaderType::m; \

    READ_ENUM(Standard)
    READ_ENUM(OpaqueLight)
    READ_ENUM(TransparentLight)
    READ_ENUM(BaseWater)
    READ_ENUM(BillieWater)
    READ_ENUM(BTSPillar)
    READ_ENUM(InterscopeConcrete)
    READ_ENUM(InterscopeCar)
    READ_ENUM(WaterfallMirror)

    getLogger().error("Unknown shader type %s", str.data());
    return Chroma::ShaderType::Standard;
}

Chroma::MaterialsManager::MaterialsManager(rapidjson::Value const &customData,
                                           TracksAD::BeatmapAssociatedData &beatmapAD, bool v2): beatmapAD(beatmapAD), v2(v2) {
    auto it = customData.FindMember(v2 ? NewConstants::V2_MATERIALS.data() : NewConstants::MATERIALS.data());

    // notice no v2 arbitrary gatekeeping? :Kek:
    if (it == customData.MemberEnd()) return;

    for (auto const& [key, val] : it->value.GetObject()) {
        materials.try_emplace(key.GetString(), CreateMaterialInfo(val));
    }
}

UnityEngine::Material *Chroma::MaterialsManager::InstantiateSharedMaterial(ShaderType shaderType) {
    static ConstString opaqueLight("Custom/OpaqueNeonLight");
    static ConstString transparentLight("Custom/TransparentNeonLight");
    static ConstString standardBTSCube("Custom/SimpleLit");
    static ConstString water("Custom/WaterLit");

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
                                                                                    "_RIMLIGHT_NONE"
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
            case ShaderType::BaseWater :
            shaderName = water;
            shaderKeywords = ArrayW<StringW>(std::initializer_list<StringW>({
                                                                                    "FOG", "HEIGHT_FOG", "INVERT_RIMLIGHT", "MASK_RED_IS_ALPHA", "NOISE_DITHERING",
                                                                                    "NORMAL_MAP", "REFLECTION_PROBE", "REFLECTION_PROBE_BOX_PROJECTION", "_DECALBLEND_ALPHABLEND",
                                                                                    "_DISSOLVEAXIS_LOCALX", "_EMISSIONCOLORTYPE_FLAT", "_EMISSIONTEXTURE_NONE",
                                                                                    "_RIMLIGHT_NONE", "_ROTATE_UV_NONE", "_VERTEXMODE_NONE", "_WHITEBOOSTTYPE_NONE",
                                                                                    "_ZWRITE_ON"
                                                                            }));
            break;
    }

    auto shader = Shader::Find(shaderName);
    auto material = Material::New_ctor(shader);

    material->set_globalIlluminationFlags(globalIlluminationFlags);
    material->set_enableInstancing(true);
    material->set_color({0,0,0,0});

    if (shaderKeywords) {
        material->set_shaderKeywords(shaderKeywords);
    }

    return material;
}

MaterialInfo Chroma::MaterialsManager::CreateMaterialInfo(rapidjson::Value const &data) {
    ArrayW<StringW> shaderKeywords;
    auto shaderKeywordsIt = data.FindMember(v2 ? NewConstants::V2_SHADER_KEYWORDS.data() : NewConstants::SHADER_KEYWORDS.data());
    if (shaderKeywordsIt != data.MemberEnd()) {
        auto arr = shaderKeywordsIt->value.GetArray();
        shaderKeywords = {arr.Size()};
        int i = 0;
        for (auto const& o : arr) {
            shaderKeywords[i] = o.GetString();
            i++;
        }
    }

    auto color = ChromaUtils::ChromaUtilities::GetColorFromData(data, v2);
    auto shaderTypeStr = ChromaUtils::getIfExists<std::string_view>(data, v2 ? NewConstants::V2_SHADER_PRESET : NewConstants::SHADER_PRESET);
    ShaderType shaderType = shaderTypeStr ? shaderTypeFromString(shaderTypeStr->data()) : ShaderType::Standard;

    std::optional<std::vector<Track*>> tracks;

    auto tracksIt = data.FindMember(v2 ? NewConstants::V2_TRACK.data() : NewConstants::TRACK.data());
    if (tracksIt != data.MemberEnd()) {
        auto size = tracksIt->value.IsString() ? 1 : tracksIt->value.Size();
        tracks.emplace().reserve(size);

        if (tracksIt->value.IsString()) {
            tracks.value().emplace_back(beatmapAD.getTrack(tracksIt->value.GetString()));
        } else if (tracksIt->value.IsArray()) {
            for (auto const& it : tracksIt->value.GetArray()) {
                tracks.value().emplace_back(beatmapAD.getTrack(it.GetString()));
            }
        }
    }

    auto material = Object::Instantiate(GetMaterialTemplate(shaderType));
    createdMaterials.emplace_back(material);
    if (color) {
        material->set_color(*color);
    }
    if (shaderKeywords) {
        material->set_shaderKeywords(shaderKeywords);
    }


    return MaterialInfo(shaderType, shaderTypeStr.value_or("Standard"), material, tracks);
}

std::optional<MaterialInfo> Chroma::MaterialsManager::GetMaterial(rapidjson::Value const &data) {
    if (data.IsString()) {
        auto it = materials.find(data.GetString());

        if (it != materials.end()) return it->second;
    } else if (data.IsObject()) {
        // mappers generating 10293029843 identical materials
        rapidjson::StringBuffer s;
        rapidjson::Writer<rapidjson::StringBuffer> writer(s);

        data.Accept(writer);

        auto jsonStr = s.GetString();
        auto it = materialsJSON.find(jsonStr);
        if (it != materialsJSON.end()) {
            return it->second;
        }

        auto material = CreateMaterialInfo(data);
        materialsJSON.emplace(jsonStr, material);
        return material;
    }

    return std::nullopt;
}

UnityEngine::Material *MaterialsManager::GetMaterialTemplate(ShaderType shaderType) {
    static SafePtrUnity<Material> _standardMaterial;
    if (!_standardMaterial) _standardMaterial = InstantiateSharedMaterial(ShaderType::Standard);

    static SafePtrUnity<Material> _opaqueLightMaterial;
    if (!_opaqueLightMaterial) _opaqueLightMaterial = InstantiateSharedMaterial(ShaderType::OpaqueLight);

    static SafePtrUnity<Material> _transparentLightMaterial;
    if(!_transparentLightMaterial) _transparentLightMaterial = InstantiateSharedMaterial(ShaderType::TransparentLight);

    static SafePtrUnity<Material> _baseWaterMaterial;
    if(!_baseWaterMaterial) _baseWaterMaterial = InstantiateSharedMaterial(ShaderType::BaseWater);

    CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Material templates {} {} {} {}", _standardMaterial.isAlive(), _opaqueLightMaterial.isAlive(), _transparentLightMaterial.isAlive(), _baseWaterMaterial.isAlive());

    Material *originalMaterial;
    switch (shaderType) {
        default:
            originalMaterial = EnvironmentMaterialManager::getMaterial(shaderType).value_or((Material *) _standardMaterial);
            break;
        case ShaderType::OpaqueLight:
            originalMaterial = (Material *) _opaqueLightMaterial;
            break;
        case ShaderType::TransparentLight:
            originalMaterial = (Material *) _transparentLightMaterial;
            break;
        case ShaderType::BaseWater:
            originalMaterial = (Material *) _baseWaterMaterial;
            break;
    }

    return originalMaterial;
}

decltype(MaterialsManager::materials) const &MaterialsManager::GetMaterials() const {
    return materials;
}

void MaterialsManager::Reset() {
    for (auto& m : createdMaterials) {
        if (!m || !m.isAlive()) continue;
        UnityEngine::Object::Destroy(const_cast<UnityEngine::Material *>(m.ptr()));
    }
    createdMaterials.clear();
}
