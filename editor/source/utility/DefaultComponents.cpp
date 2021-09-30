#include "DefaultComponents.h"

namespace
{
    const auto CubePath = std::string{"project/assets/mesh/cube.nca"};
    const auto CapsulePath = std::string{"project/assets/mesh/capsule.nca"};
    const auto SpherePath = std::string{"project/assets/mesh/sphere.nca"};

    const auto DefaultBaseColorPath = std::string{"nc/resources/texture/DefaultBaseColor.png"};
    const auto DefaultNormalPath = std::string{"nc/resources/texture/DefaultNormal.png"};
    const auto DefaultRoughnessPath = std::string{"nc/resources/texture/DefaultMetallic.png"};

    const auto DefaultMaterial = nc::graphics::Material{DefaultBaseColorPath, DefaultNormalPath, DefaultRoughnessPath};
}

namespace nc::editor
{
    void LoadDefaultResources()
    {
        graphics::LoadMeshes(std::vector<std::string>{CubePath, CapsulePath, SpherePath});
        graphics::LoadTextures(std::vector<std::string>{DefaultBaseColorPath, DefaultNormalPath, DefaultRoughnessPath});
    }

    void AddDefaultMeshRenderer(registry_type* registry, Entity entity)
    {
        registry->Add<MeshRenderer>(entity, CubePath, DefaultMaterial, graphics::TechniqueType::PhongAndUi);
    }
}