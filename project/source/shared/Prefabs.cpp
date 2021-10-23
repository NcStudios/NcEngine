#include "Prefabs.h"
#include "Assets.h"

namespace nc::sample::prefab
{
    bool IsInitialized = false;

    std::string ToString(Resource resource)
    {
        switch(resource)
        {
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
            case Resource::RampRed:
                return std::string{"RampRed"};
            case Resource::Sphere:
                return std::string{"Sphere"};
            case Resource::SphereBlue:
                return std::string{"SphereBlue"};
            case Resource::SphereGreen:
                return std::string{"SphereGreen"};
            case Resource::SphereRed:
                return std::string{"SphereRed"};
            case Resource::PlaneGreen:
                return std::string{"PlaneGreen"};
            case Resource::Table:
                return std::string{"Table"};
            case Resource::Token:
                return std::string{"Token"};
            case Resource::Worm:
                return std::string{"Worm"};
        }
        throw NcError("prefab::ToString(resource) - Unknown resource");
    }

    namespace material
    {
        Material Coin{};
        Material SolidBlue{};
        Material SolidGreen{};
        Material SolidRed{};
        Material Default{};
        Material Table{};
        Material Token{};
        Material Worm{};
    } // end namespace material

void InitializeResources()
{
    if (IsInitialized)
    {
        return;
    }

    IsInitialized = true;

    LoadSoundClipAsset("project/assets/sounds/hit.wav");
    LoadSoundClipAsset("project/assets/sounds/drums.wav");

    LoadConvexHullAsset("project/assets/mesh_colliders/coin.nca");
    LoadConcaveColliderAsset("project/assets/mesh_colliders/plane.nca");
    LoadConcaveColliderAsset("project/assets/mesh_colliders/ramp.nca");

    const std::string defaultMeshesPath = "project/assets/mesh/";
    auto meshPaths = std::vector<std::string> {  defaultMeshesPath + "capsule.nca",
                                                 defaultMeshesPath + "coin.nca",
                                                 defaultMeshesPath + "cube.nca",
                                                 defaultMeshesPath + "planet.nca",
                                                 defaultMeshesPath + "ramp.nca",
                                                 defaultMeshesPath + "sphere.nca",
                                                 defaultMeshesPath + "table.nca",
                                                 defaultMeshesPath + "token.nca",
                                                 defaultMeshesPath + "worm.nca" };
    nc::LoadMeshAssets(meshPaths);

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
    nc::LoadTextureAssets(texturePaths); 

    material::SolidBlue =  Material{ .baseColor = defaultTexturesPath + "SolidColor/Blue.png",
                                     .normal    = defaultNormal,
                                     .roughness = defaultRoughness };

    material::SolidGreen = Material{ .baseColor = defaultTexturesPath + "SolidColor/Green.png",
                                     .normal    = defaultNormal,
                                     .roughness = defaultRoughness };

    material::SolidRed =   Material{ .baseColor = defaultTexturesPath + "SolidColor/Red.png",
                                     .normal    = defaultNormal,
                                     .roughness = defaultRoughness };

    material::Coin =       Material{ .baseColor = defaultTexturesPath + "Coin/BaseColor.png",
                                     .normal    = defaultTexturesPath + "Coin/Normal.png",
                                     .roughness = defaultTexturesPath + "Coin/Roughness.png" };

    material::Default =    Material{ .baseColor = defaultBaseColor,
                                     .normal    = defaultNormal,
                                     .roughness = defaultRoughness };

    material::Table =      Material{ .baseColor = defaultTexturesPath + "Table/BaseColor.png",
                                     .normal    = defaultTexturesPath + "Table/Normal.png",
                                     .roughness = defaultTexturesPath + "Table/Roughness.png" };
     
    material::Token =      Material{ .baseColor = defaultTexturesPath + "Token/BaseColor.png",
                                     .normal    = defaultTexturesPath + "Token/Normal.png",
                                     .roughness = defaultTexturesPath + "Token/Roughness.png" };

    material::Worm =       Material{ .baseColor = defaultTexturesPath + "Logo/BaseColor.png",
                                     .normal    = defaultTexturesPath + "Logo/Normal.png",
                                     .roughness = defaultTexturesPath + "Logo/Roughness.png" };
}

template<Resource Resource_t>
Entity Create_(registry_type*, EntityInfo);

template<> Entity Create_<Resource::Capsule>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<MeshRenderer>(handle, "project/assets/mesh/capsule.nca", material::Default, TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::CapsuleBlue>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<MeshRenderer>(handle, "project/assets/mesh/capsule.nca", material::SolidBlue, TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::CapsuleGreen>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<MeshRenderer>(handle, "project/assets/mesh/capsule.nca", material::SolidGreen, TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::CapsuleRed>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<MeshRenderer>(handle, "project/assets/mesh/capsule", material::SolidRed, TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::Coin>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<MeshRenderer>(handle, "project/assets/mesh/coin.nca", material::Coin, TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::Cube>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<MeshRenderer>(handle, "project/assets/mesh/cube.nca", material::Default, TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::CubeBlue>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<MeshRenderer>(handle, "project/assets/mesh/cube.nca", material::SolidBlue, TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::CubeGreen>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<MeshRenderer>(handle, "project/assets/mesh/cube.nca", material::SolidGreen, TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::CubeRed>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<MeshRenderer>(handle, "project/assets/mesh/cube.nca", material::SolidRed, TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::Disc>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<MeshRenderer>(handle, "project/assets/mesh/coin.nca", material::Default, TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::DiscBlue>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<MeshRenderer>(handle, "project/assets/mesh/coin.nca", material::SolidBlue, TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::DiscGreen>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<MeshRenderer>(handle, "project/assets/mesh/coin.nca", material::SolidGreen, TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::DiscRed>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<MeshRenderer>(handle, "project/assets/mesh/coin.nca", material::SolidRed, TechniqueType::PhongAndUi);
    return handle;
}


template<> Entity Create_<Resource::RampRed>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<MeshRenderer>(handle, "project/assets/mesh/ramp.nca", material::SolidRed, TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::Sphere>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<MeshRenderer>(handle, "project/assets/mesh/sphere.nca", material::Default, TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::SphereBlue>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<MeshRenderer>(handle, "project/assets/mesh/sphere.nca", material::SolidBlue, TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::SphereGreen>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<MeshRenderer>(handle, "project/assets/mesh/sphere.nca", material::SolidGreen, TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::SphereRed>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<MeshRenderer>(handle, "project/assets/mesh/sphere.nca", material::SolidRed, TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::PlaneGreen>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<MeshRenderer>(handle, "project/assets/mesh/plane.nca", material::SolidGreen, TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::Table>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<MeshRenderer>(handle, "project/assets/mesh/table.nca", material::Table, TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::Token>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<MeshRenderer>(handle, "project/assets/mesh/token.nca", material::Token, TechniqueType::PhongAndUi);
    return handle;
}

template<> Entity Create_<Resource::Worm>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<MeshRenderer>(handle, "project/assets/mesh/worm.nca", material::Worm, TechniqueType::PhongAndUi);
    return handle;
}

using CreateFunc_t = Entity(*)(registry_type* registry, EntityInfo info);

const auto dispatch = std::unordered_map<prefab::Resource, CreateFunc_t>
{
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
    std::pair{Resource::RampRed,       Create_<Resource::RampRed>},
    std::pair{Resource::Sphere,        Create_<Resource::Sphere>},
    std::pair{Resource::SphereBlue,    Create_<Resource::SphereBlue>},
    std::pair{Resource::SphereGreen,   Create_<Resource::SphereGreen>},
    std::pair{Resource::SphereRed,     Create_<Resource::SphereRed>},
    std::pair{Resource::PlaneGreen,    Create_<Resource::PlaneGreen>},
    std::pair{Resource::Table,         Create_<Resource::Table>},
    std::pair{Resource::Token,         Create_<Resource::Token>},
    std::pair{Resource::Worm,          Create_<Resource::Worm>}
};

nc::Entity Create(registry_type* registry, Resource resource, EntityInfo info)
{
    return dispatch.at(resource)(registry, std::move(info));
}
} // end namespace project::prefab