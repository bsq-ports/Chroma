#include "environment_enhancements/MaterialsManager.hpp"
#include "Chroma.hpp"
#include "utils/ChromaUtils.hpp"
#include "UnityEngine/MaterialGlobalIlluminationFlags.hpp"

using namespace Chroma;
using namespace UnityEngine;

ShaderType shaderTypeFromString(std::string_view str) {
    if (str == "Standard") {
        return ShaderType::Standard;
    }
    if (str == "OpaqueLight") {
        return ShaderType::OpaqueLight;
    }
    if (str == "TransparentLight") {
        return ShaderType::TransparentLight;
    }

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

    auto color = ChromaUtils::ChromaUtilities::GetColorFromData(data, v2).value_or(Sombrero::FastColor(0,0,0,0));
    auto shaderTypeStr = ChromaUtils::getIfExists<std::string_view>(data, v2 ? NewConstants::V2_SHADER_PRESET : NewConstants::SHADER_PRESET);
    ShaderType shaderType = shaderTypeStr ? shaderTypeFromString(shaderTypeStr->data()) : ShaderType::Standard;

    std::optional<std::vector<Track*>> tracks;

    auto tracksIt = data.FindMember(NewConstants::TRACK.data());
    if (tracksIt != data.MemberEnd()) {
        auto size = tracksIt->value.IsString() ? 1 : tracksIt->value.Size();
        tracks.emplace().reserve(size);

        if (tracksIt->value.IsString()) {
            tracks.value().emplace_back(&(beatmapAD.tracks.try_emplace(tracksIt->value.GetString(), v2).first->second));
        } else if (tracksIt->value.IsArray()) {
            for (auto const& it : tracksIt->value.GetArray()) {
                tracks.value().emplace_back(&(beatmapAD.tracks.try_emplace(it.GetString(), v2).first->second));
            }
        }
    }

    auto material = Object::Instantiate(GetMaterialTemplate(shaderType));
    createdMaterials.emplace_back(material);
    material->set_color(color);
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
        return CreateMaterialInfo(data);
    }

    return std::nullopt;
}

UnityEngine::Material *MaterialsManager::GetMaterialTemplate(ShaderType shaderType) {
    static SafePtrUnity<Material> _standardMaterial;
    if (!_standardMaterial) _standardMaterial = InstantiateSharedMaterial(ShaderType::Standard);

    static SafePtrUnity<Material> _opaqueLightMaterial;
    if (!_opaqueLightMaterial) InstantiateSharedMaterial(ShaderType::OpaqueLight);

    static SafePtrUnity<Material> _transparentLightMaterial;
    if(!_transparentLightMaterial) _transparentLightMaterial = InstantiateSharedMaterial(ShaderType::TransparentLight);

    Material *originalMaterial;
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

    return originalMaterial;
}

decltype(MaterialsManager::materials) const &MaterialsManager::GetMaterials() const {
    return materials;
}
