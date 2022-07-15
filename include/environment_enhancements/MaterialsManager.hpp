#pragma once

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "tracks/shared/Animation/Track.h"
#include "tracks/shared/AssociatedData.h"

#include "UnityEngine/Material.hpp"

namespace Chroma {
    enum struct ShaderType
    {
        Standard,
        OpaqueLight,
        TransparentLight
    };

    bool IsLightType(ShaderType shaderType);

    struct MaterialInfo
    {
        MaterialInfo(ShaderType shaderType, std::string_view shaderTypeStr,
                     SafePtrUnity<UnityEngine::Material> const &material, std::optional<std::vector<Track *>> track)
                : ShaderType(shaderType), ShaderTypeStr(shaderTypeStr), Material(material), Track(std::move(track)) {
            CRASH_UNLESS(material.isAlive());
        }

        ShaderType ShaderType;
        std::string ShaderTypeStr;

        SafePtrUnity<UnityEngine::Material> Material;

        std::optional<std::vector<Track*>> Track;
    };

    struct MaterialsManager {
        std::unordered_map<std::string, const MaterialInfo> materials;
        TracksAD::BeatmapAssociatedData& beatmapAD;


        const bool v2;

        static UnityEngine::Material* InstantiateSharedMaterial(ShaderType shaderType);
        MaterialInfo CreateMaterialInfo(rapidjson::Value const& data);

        static UnityEngine::Material* GetMaterialTemplate(ShaderType shaderType);

    public:
        MaterialsManager(rapidjson::Value const& customData, TracksAD::BeatmapAssociatedData& beatmapAD, bool v2);

        std::optional<MaterialInfo> GetMaterial(rapidjson::Value const& data);

        decltype(MaterialsManager::materials) const& GetMaterials() const;
    };
}



