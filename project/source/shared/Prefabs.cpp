#include "Prefabs.h"
#include "Assets.h"
#include "graphics/Material.h"

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
        graphics::Mesh Capsule{};
        graphics::Mesh Coin{};
        graphics::Mesh Cube{};
        graphics::Mesh Planet{};
        graphics::Mesh Ramp{};
        graphics::Mesh Sphere{};
        graphics::Mesh Plane{};
        graphics::Mesh Table{};
        graphics::Mesh Token{};
        graphics::Mesh Worm{};
    } // end namespace mesh

    namespace material
    {
        graphics::Material Beeper{nullptr};
        graphics::Material Coin{nullptr};
        graphics::Material SolidBlue{nullptr};
        graphics::Material SolidGreen{nullptr};
        graphics::Material SolidRed{nullptr};
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

    LoadSoundClipAsset("project/assets/sounds/hit.wav");

    LoadConvexHullAsset("project/assets/mesh_colliders/coin.nca");
    LoadMeshColliderAsset("project/assets/mesh_colliders/plane.nca");
    LoadMeshColliderAsset("project/assets/mesh_colliders/ramp.nca");

    graphics::LoadMeshAsset("project/assets/mesh/beeper.nca");
    graphics::LoadMeshAsset("project/assets/mesh/capsule.nca");
    graphics::LoadMeshAsset("project/assets/mesh/coin.nca");
    graphics::LoadMeshAsset("project/assets/mesh/cube.nca");
    graphics::LoadMeshAsset("project/assets/mesh/planet.nca");
    graphics::LoadMeshAsset("project/assets/mesh/ramp.nca");
    graphics::LoadMeshAsset("project/assets/mesh/sphere.nca");
    graphics::LoadMeshAsset("project/assets/mesh/table.nca");
    graphics::LoadMeshAsset("project/assets/mesh/token.nca");
    graphics::LoadMeshAsset("project/assets/mesh/worm.nca");

    mesh::Beeper = graphics::Mesh{"project/assets/mesh/beeper.nca"};
    mesh::Capsule = graphics::Mesh{"project/assets/mesh/capsule.nca"};
    mesh::Coin = graphics::Mesh{"project/assets/mesh/coin.nca"};
    mesh::Cube = graphics::Mesh{"project/assets/mesh/cube.nca"};
    mesh::Plane = graphics::Mesh{"project/assets/mesh/plane.nca"};
    mesh::Planet = graphics::Mesh{"project/assets/mesh/planet.nca"};
    mesh::Ramp = graphics::Mesh{"project/assets/mesh/ramp.nca"};
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

    const std::vector<std::string> solidBlueTextures{textureDir + "SolidColor//Blue.png", defaultNormal, defaultMetallic, defaultMetallic};
    material::SolidBlue = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(solidBlueTextures, materialProperties);

    const std::vector<std::string> solidGreenTextures{textureDir + "SolidColor//Green.png", defaultNormal, defaultMetallic, defaultMetallic};
    material::SolidGreen = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(solidGreenTextures, materialProperties);

    const std::vector<std::string> solidRedTextures{textureDir + "SolidColor//Red.png", defaultNormal, defaultMetallic, defaultMetallic};
    material::SolidRed = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(solidRedTextures, materialProperties);

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

template<> Entity Create_<Resource::Capsule>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Capsule, material::Default);
    return handle;
}

template<> Entity Create_<Resource::CapsuleBlue>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Capsule, material::SolidBlue);
    return handle;
}

template<> Entity Create_<Resource::CapsuleGreen>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Capsule, material::SolidGreen);
    return handle;
}

template<> Entity Create_<Resource::CapsuleRed>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Capsule, material::SolidRed);
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
    registry->Add<Renderer>(handle, mesh::Cube, material::SolidBlue);
    return handle;
}

template<> Entity Create_<Resource::CubeGreen>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Cube, material::SolidGreen);
    return handle;
}

template<> Entity Create_<Resource::CubeRed>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Cube, material::SolidRed);
    return handle;
}

template<> Entity Create_<Resource::Disc>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Coin, material::Default);
    return handle;
}

template<> Entity Create_<Resource::DiscBlue>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Coin, material::SolidBlue);
    return handle;
}

template<> Entity Create_<Resource::DiscGreen>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Coin, material::SolidGreen);
    return handle;
}

template<> Entity Create_<Resource::DiscRed>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Coin, material::SolidRed);
    return handle;
}

template<> Entity Create_<Resource::RampRed>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Ramp, material::SolidRed);
    return handle;
}

template<> Entity Create_<Resource::Sphere>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Planet, material::Default);
    return handle;
}

template<> Entity Create_<Resource::SphereBlue>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Planet, material::SolidBlue);
    return handle;
}

template<> Entity Create_<Resource::SphereGreen>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Planet, material::SolidGreen);
    return handle;
}

template<> Entity Create_<Resource::SphereRed>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Planet, material::SolidRed);
    return handle;
}

template<> Entity Create_<Resource::PlaneGreen>(registry_type* registry, EntityInfo info)
{
    auto handle = registry->Add<Entity>(std::move(info));
    registry->Add<Renderer>(handle, mesh::Plane, material::SolidGreen);
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