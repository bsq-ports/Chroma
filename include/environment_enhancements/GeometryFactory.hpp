#pragma once

#include <unordered_map>
#include <unordered_set>
#include <string>

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "GameObjectInfo.hpp"
#include "sombrero/shared/FastColor.hpp"

#include "UnityEngine/Material.hpp"
#include "Zenject/IInstantiator.hpp"
#include "GlobalNamespace/TubeBloomPrePassLight.hpp"


namespace Chroma {


    enum struct GeometryType
    {
        Sphere,
        Capsule,
        Cylinder,
        Cube,
        Plane,
        Quad,
        Triangle
    };

    enum struct ShaderType
    {
        Standard,
        OpaqueLight,
        TransparentLight
    };

    class GeometryFactory {
    public:
        GeometryFactory();

        void reset();

        UnityEngine::GameObject * Create(rapidjson::Value const &data);

    private:
        std::optional<GlobalNamespace::TubeBloomPrePassLight*> _originalTubeBloomPrePassLight = std::nullopt;
        SafePtr<Zenject::IInstantiator> instantiator;

        static UnityEngine::Material* InstantiateSharedMaterial(ShaderType shaderType);
        static UnityEngine::Material* GetMaterial(Sombrero::FastColor const& color, ShaderType shaderType, std::optional<std::vector<std::string>> const& keywords);

        static bool IsLightType(ShaderType shaderType);
    };
}
