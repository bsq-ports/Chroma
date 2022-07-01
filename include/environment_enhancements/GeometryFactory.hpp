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
#include "MaterialsManager.hpp"


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



    class GeometryFactory {
    public:
        GeometryFactory(MaterialsManager const& materialsManager, bool v2) : materialsManager(materialsManager), v2(v2) {}

        MaterialsManager materialsManager;
        bool v2;
        void reset();


        UnityEngine::GameObject * Create(rapidjson::Value const &data);

    private:
        std::optional<GlobalNamespace::TubeBloomPrePassLight*> _originalTubeBloomPrePassLight = std::nullopt;
        SafePtr<Zenject::IInstantiator> instantiator;




    };
}
