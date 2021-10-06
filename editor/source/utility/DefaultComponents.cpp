#include "DefaultComponents.h"
#include "Assets.h"

namespace
{
    const auto CubeMeshPath             = std::string{"nc/resources/mesh/cube.nca"};
    const auto CapsuleMeshPath          = std::string{"nc/resources/mesh/capsule.nca"};
    const auto SphereMeshPath           = std::string{"nc/resources/mesh/sphere.nca"};
    const auto PlaneMeshPath            = std::string{"nc/resources/mesh/plane.nca"};
    const auto CubeHullColliderPath     = std::string{"nc/resources/hull_collider/cube.nca"};
    const auto PlaneConcaveColliderPath = std::string{"nc/resources/concave_collider/plane.nca"};
    const auto DefaultBaseColorPath     = std::string{"nc/resources/texture/DefaultBaseColor.png"};
    const auto DefaultNormalPath        = std::string{"nc/resources/texture/DefaultNormal.png"};
    const auto DefaultRoughnessPath     = std::string{"nc/resources/texture/DefaultMetallic.png"};
    const auto DefaultMaterial          = nc::graphics::Material{DefaultBaseColorPath, DefaultNormalPath, DefaultRoughnessPath};
}

namespace nc::editor
{
    void LoadDefaultResources()
    {
        graphics::LoadMeshes(std::vector<std::string>{CubeMeshPath, CapsuleMeshPath, SphereMeshPath, PlaneMeshPath});
        graphics::LoadTextures(std::vector<std::string>{DefaultBaseColorPath, DefaultNormalPath, DefaultRoughnessPath});
        LoadConvexHullAsset(CubeHullColliderPath);
        LoadConcaveColliderAsset(PlaneConcaveColliderPath);
    }

    void AddDefaultMeshRenderer(registry_type* registry, Entity entity)
    {
        registry->Add<MeshRenderer>(entity, CubeMeshPath, DefaultMaterial, graphics::TechniqueType::PhongAndUi);
    }
}