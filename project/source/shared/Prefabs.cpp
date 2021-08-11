#include "Prefabs.h"
#include "Assets.h"
#include "graphics/vulkan/Mesh.h"
#include "graphics/vulkan/Texture.h"
#include "graphics/vulkan/Material.h"
#include "graphics/vulkan/TechniqueType.h"

namespace nc::sample::prefab
{
    std::string ToString(Resource resource)
    {
        switch(resource)
        {
            case Resource::Beeper:
                return std::string{"Beeper"};
            case Resource::Capsule:
                return std::string{"Capsule"};
            case Resource::CapsuleBlue:
                return std::string{"CapsuleBlue"};
            case Resource::CapsuleGreen:
                return std::string{"CapsuleGreen"};
            case Resource::CapsuleRed:
                return std::string{"CapsuleRed"};
            case Resource::Coin:
                return std::string{"Coin"};
            case Resource::Cube:
                return std::string{"Cube"};
            case Resource::CubeBlue:
                return std::string{"CubeBlue"};
            case Resource::CubeGreen:
                return std::string{"CubeGreen"};
            case Resource::CubeRed:
                return std::string{"CubeRed"};
            case Resource::Disc:
                return std::string{"Disc"};
            case Resource::DiscBlue:
                return std::string{"DiscBlue"};
            case Resource::DiscGreen:
                return std::string{"DiscGreen"};
            case Resource::DiscRed:
                return std::string{"DiscRed"};
            case Resource::Sphere:
                return std::string{"Sphere"};
            case Resource::SphereBlue:
                return std::string{"SphereBlue"};
            case Resource::SphereGreen:
                return std::string{"Sphere Green"};
            case Resource::SphereRed:
                return std::string{"Sphere Red"};
            case Resource::Table:
                return std::string{"Table"};
            case Resource::Token:
                return std::string{"Token"};
            case Resource::WireframeCube:
                return std::string{"WireframeCube"};
            case Resource::Worm:
                return std::string{"Worm"};
        }
        throw std::runtime_error("prefab::ToString(resource) - Unknown resource");
    }

    namespace mesh
    {
        std::vector<std::string> meshPaths;
    } // end namespace mesh

    namespace material
    {
        graphics::vulkan::Material Beeper{};
        graphics::vulkan::Material Coin{};
        graphics::vulkan::Material SolidBlue{};
        graphics::vulkan::Material SolidGreen{};
        graphics::vulkan::Material SolidRed{};
        graphics::vulkan::Material Default{};
        graphics::vulkan::Material Table{};
        graphics::vulkan::Material Token{};
        graphics::vulkan::Material WireframeCube{};
        graphics::vulkan::Material Worm{};
    } // end namespace material

void InitializeResources()
{
    const std::string defaultMeshesPath = "project/assets/mesh/";
    mesh::meshPaths = std::vector<std::string> { defaultMeshesPath + "beeper.nca",
                                                 defaultMeshesPath + "capsule.nca",
                                                 defaultMeshesPath + "coin.nca",
                                                 defaultMeshesPath + "cube.nca",
                                                 defaultMeshesPath + "planet.nca",
                                                 defaultMeshesPath + "sphere.nca",
                                                 defaultMeshesPath + "table.nca",
                                                 defaultMeshesPath + "token.nca",
                                                 defaultMeshesPath + "worm.nca",
                                                 "project/assets/mesh_colliders/coin.nca" };
    nc::graphics::vulkan::LoadMeshes(mesh::meshPaths); 

    LoadHullColliderAsset("project/assets/mesh_colliders/coin.nca");

    const auto defaultBaseColor = std::string{"nc/resources/texture/DefaultBaseColor.png"};
    const auto defaultNormal    = std::string{"nc/resources/texture/DefaultNormal.png"};
    const auto defaultRoughness = std::string{"nc/resources/texture/DefaultMetallic.png"};

    const std::string defaultTexturesPath = "project/Textures/";
    const std::vector<std::string> texturePaths { defaultBaseColor, 
                                                  defaultNormal, 
                                                  defaultRoughness,
                                                  defaultTexturesPath + "Beeper/BaseColor.png",
                                                  defaultTexturesPath + "Beeper/Normal.png",
                                                  defaultTexturesPath + "Beeper/Roughness.png",
                                                  defaultTexturesPath + "SolidColor/Blue.png",
                                                  defaultTexturesPath + "SolidColor/Green.png",
                                                  defaultTexturesPath + "SolidColor/Red.png",
                                                  defaultTexturesPath + "Coin/BaseColor.png",
                                                  defaultTexturesPath + "Coin/Normal.png",
                                                  defaultTexturesPath + "Coin/Roughness.png",
                                                  defaultTexturesPath + "Table/BaseColor.png",
                                                  defaultTexturesPath + "Table/Normal.png",
                                                  defaultTexturesPath + "Table/Roughness.png",
                                                  defaultTexturesPath + "Token/BaseColor.png",
                                                  defaultTexturesPath + "Token/Normal.png",
                                                  defaultTexturesPath + "Token/Roughness.png",
                                                  defaultTexturesPath + "Logo/BaseColor.png",
                                                  defaultTexturesPath + "Logo/Normal.png",
                                                  defaultTexturesPath + "Logo/Roughness.png" };
    nc::graphics::vulkan::LoadTextures(texturePaths); 

    material::Beeper =     graphics::vulkan::Material{ .baseColor = defaultTexturesPath + "Beeper/BaseColor.png",
                                                       .normal    = defaultTexturesPath + "Beeper/Normal.png",
                                                       .roughness = defaultTexturesPath + "Beeper/Roughness.png" };
    
    material::SolidBlue =  graphics::vulkan::Material{ .baseColor = defaultTexturesPath + "SolidColor/Blue.png",
                                                       .normal    = defaultNormal,
                                                       .roughness = defaultRoughness };

    material::SolidGreen = graphics::vulkan::Material{ .baseColor = defaultTexturesPath + "SolidColor/Green.png",
                                                       .normal    = defaultNormal,
                                                       .roughness = defaultRoughness };

    material::SolidRed =   graphics::vulkan::Material{ .baseColor = defaultTexturesPath + "SolidColor/Red.png",
                                                       .normal    = defaultNormal,
                                                       .roughness = defaultRoughness };

    material::Coin =       graphics::vulkan::Material{ .baseColor = defaultTexturesPath + "Coin/BaseColor.png",
                                                       .normal    = defaultTexturesPath + "Coin/Normal.png",
                                                       .roughness = defaultTexturesPath + "Coin/Roughness.png" };

    material::Default =    graphics::vulkan::Material{ .baseColor = defaultBaseColor,
                                                       .normal    = defaultNormal,
                                                       .roughness = defaultRoughness };

    material::Table =      graphics::vulkan::Material{ .baseColor = defaultTexturesPath + "Table/BaseColor.png",
                                                       .normal    = defaultTexturesPath + "Table/Normal.png",
                                                       .roughness = defaultTexturesPath + "Table/Roughness.png" };
     
    material::Token =      graphics::vulkan::Material{ .baseColor = defaultTexturesPath + "Token/BaseColor.png",
                                                       .normal    = defaultTexturesPath + "Token/Normal.png",
                                                       .roughness = defaultTexturesPath + "Token/Roughness.png" };

    material::Worm =       graphics::vulkan::Material{ .baseColor = defaultTexturesPath + "Logo/BaseColor.png",
                                                       .normal    = defaultTexturesPath + "Logo/Normal.png",
                                                       .roughness = defaultTexturesPath + "Logo/Roughness.png" };
}

template<Resource Resource_t>
Entity Create_(registry_type*, EntityInfo);

template<> Entity Create_<Resource::Beeper>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<vulkan::MeshRenderer>(handle, mesh::meshPaths[0], material::Beeper, nc::graphics::vulkan::TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::Capsule>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<vulkan::MeshRenderer>(handle, mesh::meshPaths[1], material::Default, nc::graphics::vulkan::TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::CapsuleBlue>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<vulkan::MeshRenderer>(handle, mesh::meshPaths[1], material::SolidBlue, nc::graphics::vulkan::TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::CapsuleGreen>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<vulkan::MeshRenderer>(handle, mesh::meshPaths[1], material::SolidGreen, nc::graphics::vulkan::TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::CapsuleRed>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<vulkan::MeshRenderer>(handle, mesh::meshPaths[1], material::SolidRed, nc::graphics::vulkan::TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::Coin>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<vulkan::MeshRenderer>(handle, mesh::meshPaths[2], material::Coin, nc::graphics::vulkan::TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::Cube>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<vulkan::MeshRenderer>(handle, mesh::meshPaths[3], material::Default, nc::graphics::vulkan::TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::CubeBlue>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<vulkan::MeshRenderer>(handle, mesh::meshPaths[3], material::SolidBlue, nc::graphics::vulkan::TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::CubeGreen>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<vulkan::MeshRenderer>(handle, mesh::meshPaths[3], material::SolidGreen, nc::graphics::vulkan::TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::CubeRed>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<vulkan::MeshRenderer>(handle, mesh::meshPaths[3], material::SolidRed, nc::graphics::vulkan::TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::Disc>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<vulkan::MeshRenderer>(handle, mesh::meshPaths[2], material::Default, nc::graphics::vulkan::TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::DiscBlue>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<vulkan::MeshRenderer>(handle, mesh::meshPaths[2], material::SolidBlue, nc::graphics::vulkan::TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::DiscGreen>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<vulkan::MeshRenderer>(handle, mesh::meshPaths[2], material::SolidGreen, nc::graphics::vulkan::TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::DiscRed>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<vulkan::MeshRenderer>(handle, mesh::meshPaths[2], material::SolidRed, nc::graphics::vulkan::TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::Sphere>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<vulkan::MeshRenderer>(handle, mesh::meshPaths[4], material::Default, nc::graphics::vulkan::TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::SphereBlue>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<vulkan::MeshRenderer>(handle, mesh::meshPaths[4], material::SolidBlue, nc::graphics::vulkan::TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::SphereGreen>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<vulkan::MeshRenderer>(handle, mesh::meshPaths[4], material::SolidGreen, nc::graphics::vulkan::TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::SphereRed>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<vulkan::MeshRenderer>(handle, mesh::meshPaths[4], material::SolidRed, nc::graphics::vulkan::TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::Table>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<vulkan::MeshRenderer>(handle, mesh::meshPaths[6], material::Table, nc::graphics::vulkan::TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::Token>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<vulkan::MeshRenderer>(handle, mesh::meshPaths[7], material::Token, nc::graphics::vulkan::TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::WireframeCube>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<vulkan::MeshRenderer>(handle, mesh::meshPaths[3], material::Default, nc::graphics::vulkan::TechniqueType::Wireframe);
    return handle;
}

template<> Entity Create_<Resource::Worm>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<vulkan::MeshRenderer>(handle, mesh::meshPaths[8], material::Worm, nc::graphics::vulkan::TechniqueType::PhongAndUi);
    return handle;
}

using CreateFunc_t = Entity(*)(registry_type* registry, EntityInfo info);

const auto dispatch = std::unordered_map<prefab::Resource, CreateFunc_t>
{
    std::pair{Resource::Beeper,        Create_<Resource::Beeper>},
    std::pair{Resource::Capsule,       Create_<Resource::Capsule>},
    std::pair{Resource::CapsuleBlue,   Create_<Resource::CapsuleBlue>},
    std::pair{Resource::CapsuleGreen,  Create_<Resource::CapsuleGreen>},
    std::pair{Resource::CapsuleRed,    Create_<Resource::CapsuleRed>},
    std::pair{Resource::Coin,          Create_<Resource::Coin>},
    std::pair{Resource::Cube,          Create_<Resource::Cube>},
    std::pair{Resource::CubeBlue,      Create_<Resource::CubeBlue>},
    std::pair{Resource::CubeGreen,     Create_<Resource::CubeGreen>},
    std::pair{Resource::CubeRed,       Create_<Resource::CubeRed>},
    std::pair{Resource::Disc,          Create_<Resource::Disc>},
    std::pair{Resource::DiscBlue,      Create_<Resource::DiscBlue>},
    std::pair{Resource::DiscGreen,     Create_<Resource::DiscGreen>},
    std::pair{Resource::DiscRed,       Create_<Resource::DiscRed>},
    std::pair{Resource::Sphere,        Create_<Resource::Sphere>},
    std::pair{Resource::SphereBlue,    Create_<Resource::SphereBlue>},
    std::pair{Resource::SphereGreen,   Create_<Resource::SphereGreen>},
    std::pair{Resource::SphereRed,     Create_<Resource::SphereRed>},
    std::pair{Resource::Table,         Create_<Resource::Table>},
    std::pair{Resource::Token,         Create_<Resource::Token>},
    std::pair{Resource::WireframeCube, Create_<Resource::WireframeCube>},
    std::pair{Resource::Worm,          Create_<Resource::Worm>}
};

nc::Entity Create(registry_type* registry, Resource resource, EntityInfo info)
{
    return dispatch.at(resource)(registry, std::move(info));
}
} // end namespace project::prefab