#include "graphics/Material.h"
#include "Prefabs.h"

namespace
{
    bool isInitialized = false;
}

namespace nc::sample::prefab
{
    std::string ToString(Resource resource)
    {
        switch(resource)
        {
            case Resource::Beeper:
                return std::string{"Beeper"};
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
            case Resource::Sphere:
                return std::string{"Sphere"};
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
        graphics::Mesh Beeper{};
        graphics::Mesh Coin{};
        graphics::Mesh Cube{};
        graphics::Mesh Sphere{};
        graphics::Mesh Table{};
        graphics::Mesh Token{};
        graphics::Mesh Worm{};
    } // end namespace mesh

    namespace material
    {
        graphics::Material Beeper{nullptr};
        graphics::Material Coin{nullptr};
        graphics::Material CubeBlue{nullptr};
        graphics::Material CubeGreen{nullptr};
        graphics::Material CubeRed{nullptr};
        graphics::Material Default{nullptr};
        graphics::Material Table{nullptr};
        graphics::Material Token{nullptr};
        graphics::Material WireframeCube{nullptr};
        graphics::Material Worm{nullptr};
    } // end namespace material

void InitializeResources()
{
    if(isInitialized)
        return;

    isInitialized = true;

    graphics::LoadMeshAsset("project/assets/mesh/beeper.nca");
    graphics::LoadMeshAsset("project/assets/mesh/coin.nca");
    graphics::LoadMeshAsset("project/assets/mesh/cube.nca");
    graphics::LoadMeshAsset("project/assets/mesh/sphere.nca");
    graphics::LoadMeshAsset("project/assets/mesh/table.nca");
    graphics::LoadMeshAsset("project/assets/mesh/token.nca");
    graphics::LoadMeshAsset("project/assets/mesh/worm.nca");

    mesh::Beeper = graphics::Mesh{"project/assets/mesh/beeper.nca"};
    mesh::Coin = graphics::Mesh{"project/assets/mesh/coin.nca"};
    mesh::Cube = graphics::Mesh{"project/assets/mesh/cube.nca"};
    mesh::Sphere = graphics::Mesh{"project/assets/mesh/sphere.nca"};
    mesh::Table = graphics::Mesh{"project/assets/mesh/table.nca"};
    mesh::Token = graphics::Mesh{"project/assets/mesh/token.nca"};
    mesh::Worm = graphics::Mesh{"project/assets/mesh/worm.nca"};

    auto materialProperties = graphics::MaterialProperties{};
    const auto textureDir = std::string{"project//Textures//"};
    const auto defaultBaseColor = std::string{"nc//resources//texture//DefaultBaseColor.png"};
    const auto defaultNormal = std::string{"nc//resources//texture//DefaultNormal.png"};
    const auto defaultMetallic = std::string{"nc//resources//texture//DefaultMetallic.png"};

    const std::vector<std::string> beeperTextures{textureDir + "Beeper//BaseColor.png", textureDir + "Beeper//Normal.png", textureDir + "Beeper//Roughness.png", defaultMetallic};
    material::Beeper = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(beeperTextures, materialProperties);

    const std::vector<std::string> cubeBlueTextures{textureDir + "SolidColor//Blue.png", defaultNormal, defaultMetallic, defaultMetallic};
    material::CubeBlue = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(cubeBlueTextures, materialProperties);

    const std::vector<std::string> cubeGreenTextures{textureDir + "SolidColor//Green.png", defaultNormal, defaultMetallic, defaultMetallic};
    material::CubeGreen = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(cubeGreenTextures, materialProperties);

    const std::vector<std::string> cubeRedTextures{textureDir + "SolidColor//Red.png", defaultNormal, defaultMetallic, defaultMetallic};
    material::CubeRed = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(cubeRedTextures, materialProperties);

    const std::vector<std::string> coinTextures{textureDir + "Coin//BaseColor.png", textureDir + "Coin//Normal.png", textureDir + "Coin//Roughness.png", defaultMetallic};
    material::Coin = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(coinTextures, materialProperties);

    const std::vector<std::string> defaultTextures{defaultBaseColor, defaultNormal, defaultMetallic, defaultMetallic};
    material::Default = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(defaultTextures, materialProperties);

    const std::vector<std::string> tableTextures{textureDir + "Table//BaseColor.png", textureDir + "Table//Normal.png", textureDir + "Table//Roughness.png", defaultMetallic};
    material::Table = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(tableTextures, materialProperties);

    const std::vector<std::string> tokenTextures{textureDir + "Token//BaseColor.png", textureDir + "Token//Normal.png", textureDir + "Token//Roughness.png", defaultMetallic};
    material::Token = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(tokenTextures, materialProperties);

    material::WireframeCube = graphics::Material::CreateMaterial<graphics::TechniqueType::Wireframe>();

    const std::vector<std::string> wormTextures{textureDir + "Logo//BaseColor.png", textureDir + "Logo//Normal.png", textureDir + "Logo//Roughness.png", defaultMetallic};
    material::Worm = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(wormTextures, materialProperties);
}

template<Resource Resource_t>
EntityHandle Create_(EntityInfo info);

template<> EntityHandle Create_<Resource::Beeper>(EntityInfo info)
{
    auto handle = CreateEntity(std::move(info));
    AddComponent<Renderer>(handle, mesh::Beeper, material::Beeper);
    return handle;
}

template<> EntityHandle Create_<Resource::Coin>(EntityInfo info)
{
    auto handle = CreateEntity(std::move(info));
    AddComponent<Renderer>(handle, mesh::Coin, material::Coin);
    return handle;
}

template<> EntityHandle Create_<Resource::Cube>(EntityInfo info)
{
    auto handle = CreateEntity(std::move(info));
    AddComponent<Renderer>(handle, mesh::Cube, material::Default);
    return handle;
}

template<> EntityHandle Create_<Resource::CubeBlue>(EntityInfo info)
{
    auto handle = CreateEntity(std::move(info));
    AddComponent<Renderer>(handle, mesh::Cube, material::CubeBlue);
    return handle;
}

template<> EntityHandle Create_<Resource::CubeGreen>(EntityInfo info)
{
    auto handle = CreateEntity(std::move(info));
    AddComponent<Renderer>(handle, mesh::Cube, material::CubeGreen);
    return handle;
}

template<> EntityHandle Create_<Resource::CubeRed>(EntityInfo info)
{
    auto handle = CreateEntity(std::move(info));
    AddComponent<Renderer>(handle, mesh::Cube, material::CubeRed);
    return handle;
}

template<> EntityHandle Create_<Resource::Sphere>(EntityInfo info)
{
    auto handle = CreateEntity(std::move(info));
    AddComponent<Renderer>(handle, mesh::Sphere, material::Default);
    return handle;
}

template<> EntityHandle Create_<Resource::Table>(EntityInfo info)
{
    auto handle = CreateEntity(std::move(info));
    AddComponent<Renderer>(handle, mesh::Table, material::Table);
    return handle;
}

template<> EntityHandle Create_<Resource::Token>(EntityInfo info)
{
    auto handle = CreateEntity(std::move(info));
    AddComponent<Renderer>(handle, mesh::Token, material::Token);
    return handle;
}

template<> EntityHandle Create_<Resource::WireframeCube>(EntityInfo info)
{
    auto handle = CreateEntity(std::move(info));
    AddComponent<Renderer>(handle, mesh::Cube, material::WireframeCube);
    return handle;
}

template<> EntityHandle Create_<Resource::Worm>(EntityInfo info)
{
    auto handle = CreateEntity(std::move(info));
    AddComponent<Renderer>(handle, mesh::Worm, material::Worm);
    return handle;
}

using CreateFunc_t = EntityHandle(*)(EntityInfo info);

const auto dispatch = std::unordered_map<prefab::Resource, CreateFunc_t>
{
    std::pair{Resource::Beeper,        Create_<Resource::Beeper>},
    std::pair{Resource::Coin,          Create_<Resource::Coin>},
    std::pair{Resource::Cube,          Create_<Resource::Cube>},
    std::pair{Resource::CubeBlue,      Create_<Resource::CubeBlue>},
    std::pair{Resource::CubeGreen,     Create_<Resource::CubeGreen>},
    std::pair{Resource::CubeRed,       Create_<Resource::CubeRed>},
    std::pair{Resource::Sphere,        Create_<Resource::Sphere>},
    std::pair{Resource::Table,         Create_<Resource::Table>},
    std::pair{Resource::Token,         Create_<Resource::Token>},
    std::pair{Resource::WireframeCube, Create_<Resource::WireframeCube>},
    std::pair{Resource::Worm,          Create_<Resource::Worm>}
};

nc::EntityHandle Create(Resource resource, EntityInfo info)
{
    return dispatch.at(resource)(std::move(info));
}
} // end namespace project::prefab