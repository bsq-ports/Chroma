#pragma once

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "tracks/shared/Animation/Track.h"
#include "tracks/shared/AssociatedData.h"

namespace UnityEngine {
    class Material;
}

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
        MaterialInfo(
                ShaderType shaderType,
                std::string_view shaderTypeStr,
                UnityEngine::Material* material,
                std::optional<std::vector<Track*>> const& track) : ShaderTypeStr(shaderTypeStr)
        {
            ShaderType = shaderType;
            Material = material;
            if (track)
            Track = track;
        }

        ShaderType ShaderType;
        std::string ShaderTypeStr;

        SafePtr<UnityEngine::Material> Material;

        std::optional<std::vector<Track*>> Track;
    };

    struct MaterialsManager {
        MaterialsManager(rapidjson::Value const& customData, TracksAD::BeatmapAssociatedData& beatmapAD, bool v2);

        std::optional<ByRef<MaterialInfo const>> GetMaterial(rapidjson::Value const& data);

    private:
        std::unordered_map<std::string, const MaterialInfo> materials;
        TracksAD::BeatmapAssociatedData& beatmapAD;
        const bool v2;


        static UnityEngine::Material* InstantiateSharedMaterial(ShaderType shaderType);

        MaterialInfo CreateMaterialInfo(rapidjson::Value const& data);
        UnityEngine::Material* GetMaterialTemplate(ShaderType shaderType);
    };
}



