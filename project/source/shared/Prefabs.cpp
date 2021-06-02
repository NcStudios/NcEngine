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
            case Resource::Sun:
                return std::string{"Sun"};
            case Resource::Mercury:
                return std::string{"Mercury"};
            case Resource::Venus:
                return std::string{"Venus"};
            case Resource::Earth:
                return std::string{"Earth"};
            case Resource::Mars:
                return std::string{"Mars"};
            case Resource::Jupiter:
                return std::string{"Jupiter"};
            case Resource::Saturn:
                return std::string{"Saturn"};
            case Resource::Uranus:
                return std::string{"Uranus"};
            case Resource::Neptune:
                return std::string{"Neptune"};
            case Resource::Pluto:
                return std::string{"Pluto"};
        }
        throw std::runtime_error("prefab::ToString(resource) - Unknown resource");
    }

    namespace mesh
    {
        graphics::Mesh Beeper{};
        graphics::Mesh Coin{};
        graphics::Mesh Cube{};
        graphics::Mesh Planet{};
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
        graphics::Material Sun{nullptr};
        graphics::Material Mercury{nullptr};
        graphics::Material Venus{nullptr};
        graphics::Material Earth{nullptr};
        graphics::Material Mars{nullptr};
        graphics::Material Jupiter{nullptr};
        graphics::Material Saturn{nullptr};
        graphics::Material Uranus{nullptr};
        graphics::Material Neptune{nullptr};
        graphics::Material Pluto{nullptr};
    } // end namespace material

void InitializeResources()
{
    if(isInitialized)
        return;

    isInitialized = true;

    graphics::LoadMeshAsset("project/assets/mesh/beeper.nca");
    graphics::LoadMeshAsset("project/assets/mesh/coin.nca");
    graphics::LoadMeshAsset("project/assets/mesh/cube.nca");
    graphics::LoadMeshAsset("project/assets/mesh/planet.nca");
    graphics::LoadMeshAsset("project/assets/mesh/sphere.nca");
    graphics::LoadMeshAsset("project/assets/mesh/table.nca");
    graphics::LoadMeshAsset("project/assets/mesh/token.nca");
    graphics::LoadMeshAsset("project/assets/mesh/worm.nca");

    mesh::Beeper = graphics::Mesh{"project/assets/mesh/beeper.nca"};
    mesh::Coin = graphics::Mesh{"project/assets/mesh/coin.nca"};
    mesh::Cube = graphics::Mesh{"project/assets/mesh/cube.nca"};
    mesh::Planet = graphics::Mesh{"project/assets/mesh/planet.nca"};
    mesh::Sphere = graphics::Mesh{"project/assets/mesh/sphere.nca"};
    mesh::Table = graphics::Mesh{"project/assets/mesh/table.nca"};
    mesh::Token = graphics::Mesh{"project/assets/mesh/token.nca"};
    mesh::Worm = graphics::Mesh{"project/assets/mesh/worm.nca"};

    const auto materialProperties = graphics::MaterialProperties{};
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

    const std::vector<std::string> sunTextures{textureDir + "Planets/Sun/BaseColor.png", textureDir + "Planets/Sun/Normal.png", textureDir + "Planets/Sun/Roughness.png", defaultMetallic};
    material::Sun = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(sunTextures, materialProperties);

    const std::vector<std::string> mercuryTextures{textureDir + "Planets/Mercury/BaseColor.png", textureDir + "Planets/Mercury/Normal.png", textureDir + "Planets/Mercury/Roughness.png", defaultMetallic};
    material::Mercury = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(mercuryTextures, materialProperties);

    const std::vector<std::string> venusTextures{textureDir + "Planets/Venus/BaseColor.png", textureDir + "Planets/Venus/Normal.png", textureDir + "Planets/Venus/Roughness.png", defaultMetallic};
    material::Venus = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(venusTextures, materialProperties);

    const std::vector<std::string> earthTextures{textureDir + "Planets/Earth/BaseColor.png", textureDir + "Planets/Earth/Normal.png", textureDir + "Planets/Earth/Roughness.png", defaultMetallic};
    material::Earth = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(earthTextures, materialProperties);

    const std::vector<std::string> marsTextures{textureDir + "Planets/Mars/BaseColor.png", textureDir + "Planets/Mars/Normal.png", textureDir + "Planets/Mars/Roughness.png", defaultMetallic};
    material::Mars = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(marsTextures, materialProperties);

    const std::vector<std::string> jupiterTextures{textureDir + "Planets/Jupiter/BaseColor.png", textureDir + "Planets/Jupiter/Normal.png", textureDir + "Planets/Jupiter/Roughness.png", defaultMetallic};
    material::Jupiter = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(jupiterTextures, materialProperties);

    const std::vector<std::string> saturnTextures{textureDir + "Planets/Saturn/BaseColor.png", textureDir + "Planets/Saturn/Normal.png", textureDir + "Planets/Saturn/Roughness.png", defaultMetallic};
    material::Saturn = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(saturnTextures, materialProperties);

    const std::vector<std::string> uranusTextures{textureDir + "Planets/Uranus/BaseColor.png", textureDir + "Planets/Uranus/Normal.png", textureDir + "Planets/Uranus/Roughness.png", defaultMetallic};
    material::Uranus = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(uranusTextures, materialProperties);

    const std::vector<std::string> neptuneTextures{textureDir + "Planets/Neptune/BaseColor.png", textureDir + "Planets/Neptune/Normal.png", textureDir + "Planets/Neptune/Roughness.png", defaultMetallic};
    material::Neptune = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(neptuneTextures, materialProperties);

    const std::vector<std::string> plutoTextures{textureDir + "Planets/Pluto/BaseColor.png", textureDir + "Planets/Pluto/Normal.png", textureDir + "Planets/Pluto/Roughness.png", defaultMetallic};
    material::Pluto = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(plutoTextures, materialProperties);
}

template<Resource Resource_t>
Entity Create_(registry_type*, EntityInfo);

template<> Entity Create_<Resource::Beeper>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Beeper, material::Beeper);
    return handle;
}

template<> Entity Create_<Resource::Coin>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Coin, material::Coin);
    return handle;
}

template<> Entity Create_<Resource::Cube>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Cube, material::Default);
    return handle;
}

template<> Entity Create_<Resource::CubeBlue>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Cube, material::CubeBlue);
    return handle;
}

template<> Entity Create_<Resource::CubeGreen>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Cube, material::CubeGreen);
    return handle;
}

template<> Entity Create_<Resource::CubeRed>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Cube, material::CubeRed);
    return handle;
}

template<> Entity Create_<Resource::Sphere>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Sphere, material::Default);
    return handle;
}

template<> Entity Create_<Resource::Table>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Table, material::Table);
    return handle;
}

template<> Entity Create_<Resource::Token>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Token, material::Token);
    return handle;
}

template<> Entity Create_<Resource::WireframeCube>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Cube, material::WireframeCube);
    return handle;
}

template<> Entity Create_<Resource::Worm>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Worm, material::Worm);
    return handle;
}

template<> Entity Create_<Resource::Sun>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Planet, material::Sun);
    return handle;
}

template<> Entity Create_<Resource::Mercury>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Planet, material::Mercury);
    return handle;
}

template<> Entity Create_<Resource::Venus>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Planet, material::Venus);
    return handle;
}

template<> Entity Create_<Resource::Earth>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Planet, material::Earth);
    return handle;
}

template<> Entity Create_<Resource::Mars>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Planet, material::Mars);
    return handle;
}

template<> Entity Create_<Resource::Jupiter>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Planet, material::Jupiter);
    return handle;
}

template<> Entity Create_<Resource::Saturn>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Planet, material::Saturn);
    return handle;
}

template<> Entity Create_<Resource::Uranus>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Planet, material::Uranus);
    return handle;
}

template<> Entity Create_<Resource::Neptune>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Planet, material::Neptune);
    return handle;
}

template<> Entity Create_<Resource::Pluto>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Planet, material::Pluto);
    return handle;
}
using CreateFunc_t = Entity(*)(registry_type* registry, EntityInfo info);

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
    std::pair{Resource::Worm,          Create_<Resource::Worm>},
    std::pair{Resource::Sun,           Create_<Resource::Sun>},
    std::pair{Resource::Mercury,       Create_<Resource::Mercury>},
    std::pair{Resource::Venus,         Create_<Resource::Venus>},
    std::pair{Resource::Earth,         Create_<Resource::Earth>},
    std::pair{Resource::Mars,          Create_<Resource::Mars>},
    std::pair{Resource::Jupiter,       Create_<Resource::Jupiter>},
    std::pair{Resource::Saturn,        Create_<Resource::Saturn>},
    std::pair{Resource::Uranus,        Create_<Resource::Uranus>},
    std::pair{Resource::Neptune,       Create_<Resource::Neptune>},
    std::pair{Resource::Pluto,         Create_<Resource::Pluto>}
};

nc::Entity Create(registry_type* registry, Resource resource, EntityInfo info)
{
    return dispatch.at(resource)(registry, std::move(info));
}
} // end namespace project::prefab