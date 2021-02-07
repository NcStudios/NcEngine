#include "graphics/Material.h"
#include "Prefabs.h"

namespace
{
    bool isInitialized = false;
}

namespace nc::sample::prefab
{
    namespace mesh
    {
        graphics::Mesh Beeper{};
        graphics::Mesh Coin{};
        graphics::Mesh Cube{};
        graphics::Mesh Table{};
        graphics::Mesh Token{};
        graphics::Mesh Worm{};
    } // end namespace mesh

    namespace material
    {
        graphics::Material Beeper{nullptr};
        graphics::Material Coin{nullptr};
        graphics::Material Cube{nullptr};
        graphics::Material Table{nullptr};
        graphics::Material Token{nullptr};
        graphics::Material WireframeCube{nullptr};
        graphics::Material Worm{nullptr};
    } // end namespace material

void InitializeResources()
{
    if(isInitialized)
    {
        return;
    }
    isInitialized = true;

    mesh::Beeper = graphics::Mesh{"project//Models//beeper.fbx"};
    mesh::Coin = graphics::Mesh{"project//Models//coin.fbx"};
    mesh::Cube = graphics::Mesh{"project//Models//cube.fbx"};
    mesh::Table = graphics::Mesh{"project//Models//table.fbx"};
    mesh::Token = graphics::Mesh{"project//Models//token.fbx"};
    mesh::Worm = graphics::Mesh{"project//Models//worm.fbx"};

    auto materialProperties = graphics::MaterialProperties{};
    auto textureDir = std::string{"project//Textures//"};
    auto defaultAlbedo = std::string{"nc//source//graphics//DefaultTexture.png"};
    auto defaultNormal = std::string{"nc//source//graphics//DefaultTexture_Normal.png"};
    auto defaultMetallic = std::string{"nc//source//graphics//DefaultTexture_Metallic.png"};

    const std::vector<std::string> beeperTextures{textureDir + "Beeper//BaseColor.png", textureDir + "Beeper//Normal.png", textureDir + "Beeper//Roughness.png", defaultMetallic};
    material::Beeper = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(beeperTextures, materialProperties);

    const std::vector<std::string> cubeTextures{defaultAlbedo, defaultNormal, defaultAlbedo, defaultMetallic};
    material::Cube = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(cubeTextures, materialProperties);

    const std::vector<std::string> coinTextures{textureDir + "Coin//BaseColor.png", textureDir + "Coin//Normal.png", textureDir + "Coin//Roughness.png", defaultMetallic};
    material::Coin = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(coinTextures, materialProperties);

    const std::vector<std::string> tableTextures{textureDir + "Table//BaseColor.png", textureDir + "Table//Normal.png", textureDir + "Table//Roughness.png", defaultMetallic};
    material::Table = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(tableTextures, materialProperties);

    const std::vector<std::string> tokenTextures{textureDir + "Token//BaseColor.png", textureDir + "Token//Normal.png", textureDir + "Token//Roughness.png", defaultMetallic};
    material::Token = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(tokenTextures, materialProperties);

    material::WireframeCube = graphics::Material::CreateMaterial<graphics::TechniqueType::Wireframe>();

    const std::vector<std::string> wormTextures{textureDir + "Logo//BaseColor.png", textureDir + "Logo//Normal.png", textureDir + "Logo//Roughness.png", defaultMetallic};
    material::Worm = graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShading>(wormTextures, materialProperties);
}

template<Resource Resource_t>
EntityHandle Create_(Vector3, Quaternion, Vector3, std::string);

template<> EntityHandle Create_<Resource::Beeper>(Vector3 position, Quaternion rotation, Vector3 scale, std::string tag)
{
    auto handle = CreateEntity(position, rotation, scale, tag);
    AddComponent<Renderer>(handle, mesh::Beeper, material::Beeper);
    return handle;
}

template<> EntityHandle Create_<Resource::Coin>(Vector3 position, Quaternion rotation, Vector3 scale, std::string tag)
{
    auto handle = CreateEntity(position, rotation, scale, tag);
    AddComponent<Renderer>(handle, mesh::Coin, material::Coin);
    return handle;
}

template<> EntityHandle Create_<Resource::Cube>(Vector3 position, Quaternion rotation, Vector3 scale, std::string tag)
{
    auto handle = CreateEntity(position, rotation, scale, tag);
    AddComponent<Renderer>(handle, mesh::Cube, material::Cube);
    return handle;
}

template<> EntityHandle Create_<Resource::Table>(Vector3 position, Quaternion rotation, Vector3 scale, std::string tag)
{
    auto handle = CreateEntity(position, rotation, scale, tag);
    AddComponent<Renderer>(handle, mesh::Table, material::Table);
    return handle;
}

template<> EntityHandle Create_<Resource::Token>(Vector3 position, Quaternion rotation, Vector3 scale, std::string tag)
{
    auto handle = CreateEntity(position, rotation, scale, tag);
    AddComponent<Renderer>(handle, mesh::Token, material::Token);
    return handle;
}

template<> EntityHandle Create_<Resource::WireframeCube>(Vector3 position, Quaternion rotation, Vector3 scale, std::string tag)
{
    auto handle = CreateEntity(position, rotation, scale, tag);
    AddComponent<Renderer>(handle, mesh::Cube, material::WireframeCube);
    return handle;
}

template<> EntityHandle Create_<Resource::Worm>(Vector3 position, Quaternion rotation, Vector3 scale, std::string tag)
{
    auto handle = CreateEntity(position, rotation, scale, tag);
    AddComponent<Renderer>(handle, mesh::Worm, material::Worm);
    return handle;
}

using CreateFunc_t = EntityHandle(*)(Vector3, Quaternion, Vector3, std::string);

const auto dispatch = std::unordered_map<prefab::Resource, CreateFunc_t>
{
    std::pair{Resource::Beeper,        Create_<Resource::Beeper>},
    std::pair{Resource::Coin,          Create_<Resource::Coin>  },
    std::pair{Resource::Cube,          Create_<Resource::Cube>  },
    std::pair{Resource::Table,         Create_<Resource::Table> },
    std::pair{Resource::Token,         Create_<Resource::Token> },
    std::pair{Resource::WireframeCube, Create_<Resource::WireframeCube> },
    std::pair{Resource::Worm,          Create_<Resource::Worm>  }
};

nc::EntityHandle Create(Resource resource, nc::Vector3 position, nc::Quaternion rotation, nc::Vector3 scale, std::string tag)
{
    return dispatch.at(resource)(position, rotation, scale, std::move(tag));
}
} // end namespace project::prefab