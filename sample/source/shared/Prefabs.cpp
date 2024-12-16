#include "Prefabs.h"

#include "ncengine/asset/Assets.h"
#include "ncengine/config/Config.h"
#include "ncutility/Hash.h"

#include <filesystem>

namespace nc::sample
{
bool IsInitialized = false;

namespace mesh
{
asset::MeshView Cube{};
asset::MeshView Sphere{};
asset::MeshView Capsule{};
asset::MeshView Plane{};
asset::MeshView HalfPipe{};
asset::MeshView Ramp{};
asset::MeshView Ogre{};
asset::MeshView Skeleton{};
asset::MeshView Cave{};
} // namespace mesh

namespace material
{
MaterialDesc Default{"DefaultMaterial"};
MaterialDesc Red{"RedMaterial"};
MaterialDesc Green{"GreenMaterial"};
MaterialDesc Blue{"BlueMaterial"};
MaterialDesc Orange{"OrangeMaterial"};
MaterialDesc Purple{"PurpleMaterial"};
MaterialDesc Teal{"TealMaterial"};
MaterialDesc Yellow{"YellowMaterial"};
MaterialDesc Ogre{"OgreMaterial"};
MaterialDesc Skeleton{"SkeletonMaterial"};
MaterialDesc Cave{"CaveMaterial"};
} // namespace material

namespace animation
{
uint64_t OgreIdle{utility::Fnv1a(std::filesystem::path("ogre\\idle.nca").string())};
uint64_t OgreAttack{utility::Fnv1a(std::filesystem::path("ogre\\attack.nca").string())};
uint64_t SkeletonIdle{utility::Fnv1a(std::filesystem::path("skeleton\\idle.nca").string())};
uint64_t SkeletonJump{utility::Fnv1a(std::filesystem::path("skeleton\\jump.nca").string())};
uint64_t SkeletonWalkRight{utility::Fnv1a(std::filesystem::path("skeleton\\walk_right.nca").string())};
uint64_t SkeletonWalkLeft{utility::Fnv1a(std::filesystem::path("skeleton\\walk_left.nca").string())};
uint64_t SkeletonWalkForward{utility::Fnv1a(std::filesystem::path("skeleton\\walk_forward.nca").string())};
uint64_t SkeletonWalkBackward{utility::Fnv1a(std::filesystem::path("skeleton\\walk_back.nca").string())};
} // namespace animation

asset::FontInfo UIFont{"SourceCodePro-Regular.ttf", 16.0f};

template<class LoadFunc>
void LoadAssets(const std::filesystem::path& rootDir, asset::asset_flags_type flags, LoadFunc load)
{
    auto paths = std::vector<std::string>{};
    for (auto&& entry : std::filesystem::recursive_directory_iterator{rootDir})
    {
        if (entry.path().extension() != ".nca")
        {
            continue;
        }

        const auto& path = entry.path();
        auto trimmedPath = std::filesystem::path{};
        auto segment = path.begin();
        ++segment; ++segment;
        for (; segment != path.end(); ++segment)
        {
            trimmedPath /= *segment;
        }

        paths.push_back(trimmedPath.string());
    }

    load(paths, false, flags);
}

void InitializeResources()
{
    if (IsInitialized)
    {
        return;
    }

    IsInitialized = true;

    const auto& assetSettings = config::GetAssetSettings();
    LoadAssets(assetSettings.audioClipsPath, asset::AssetFlags::None, &asset::LoadAudioClipAssets);
    LoadAssets(assetSettings.concaveCollidersPath, asset::AssetFlags::None, &asset::LoadConcaveColliderAssets);
    LoadAssets(assetSettings.cubeMapsPath, asset::AssetFlags::None, &asset::LoadCubeMapAssets);
    LoadAssets(assetSettings.hullCollidersPath, asset::AssetFlags::None, &asset::LoadConvexHullAssets);
    LoadAssets(assetSettings.meshesPath, asset::AssetFlags::None, &asset::LoadMeshAssets);
    LoadAssets(assetSettings.skeletalAnimationsPath, asset::AssetFlags::None, &asset::LoadSkeletalAnimationAssets);
    LoadFont(UIFont);

    std::vector<std::string> textures
    {
        "solid_color/Blue.nca",
        "solid_color/Green.nca",
        "solid_color/Red.nca",
        "solid_color/Orange.nca",
        "solid_color/Purple.nca",
        "solid_color/Teal.nca",
        "solid_color/Yellow.nca",
        "ogre/BaseColor.nca",
        "skeleton/BaseColor.nca",
        "cave/BaseColor.nca"
    };

    asset::LoadTextureAssets(textures, false, asset::AssetFlags::TextureTypeImage);

    std::vector<std::string> normalMaps
    {
        "ogre/Normal.nca",
        "skeleton/Normal.nca",
        "cave/Normal.nca"
    };

    asset::LoadTextureAssets(normalMaps, false, asset::AssetFlags::TextureTypeNormalMap);

    asset::LoadCubeMapAsset(cubemap::NightSkyPath);
}

void ReloadPrefabs()
{
    mesh::Cube = asset::AcquireMeshAsset(asset::CubeMesh);
    mesh::Sphere = asset::AcquireMeshAsset(asset::SphereMesh);
    mesh::Capsule = asset::AcquireMeshAsset(asset::CapsuleMesh);
    mesh::Plane = asset::AcquireMeshAsset(asset::PlaneMesh);
    mesh::Ramp = asset::AcquireMeshAsset(mesh::RampPath);
    mesh::HalfPipe = asset::AcquireMeshAsset(mesh::HalfPipePath);
    mesh::Ogre = asset::AcquireMeshAsset(mesh::OgrePath);
    mesh::Skeleton = asset::AcquireMeshAsset(mesh::SkeletonPath);
    mesh::Cave = asset::AcquireMeshAsset(mesh::CavePath);

    const auto normal = asset::AcquireTextureAsset(asset::DefaultNormal);
    material::Default.properties.diffuseTexture = asset::AcquireTextureAsset(asset::DefaultBaseColor);
    material::Default.properties.normalTexture = normal;
    material::Red.properties.diffuseTexture = asset::AcquireTextureAsset("solid_color/Red.nca");
    material::Red.properties.normalTexture = normal;
    material::Green.properties.diffuseTexture = asset::AcquireTextureAsset("solid_color/Green.nca");
    material::Green.properties.normalTexture = normal;
    material::Blue.properties.diffuseTexture = asset::AcquireTextureAsset("solid_color/Blue.nca");
    material::Blue.properties.normalTexture = normal;
    material::Orange.properties.diffuseTexture = asset::AcquireTextureAsset("solid_color/Orange.nca");
    material::Orange.properties.normalTexture = normal;
    material::Purple.properties.diffuseTexture = asset::AcquireTextureAsset("solid_color/Purple.nca");
    material::Purple.properties.normalTexture = normal;
    material::Teal.properties.diffuseTexture = asset::AcquireTextureAsset("solid_color/Teal.nca");
    material::Teal.properties.normalTexture = normal;
    material::Yellow.properties.diffuseTexture = asset::AcquireTextureAsset("solid_color/Yellow.nca");
    material::Yellow.properties.normalTexture = normal;
    material::Ogre.properties.diffuseTexture = asset::AcquireTextureAsset("ogre/BaseColor.nca");
    material::Ogre.properties.normalTexture = asset::AcquireTextureAsset("ogre/Normal.nca");
    material::Skeleton.properties.diffuseTexture = asset::AcquireTextureAsset("skeleton/BaseColor.nca");
    material::Skeleton.properties.normalTexture = asset::AcquireTextureAsset("skeleton/Normal.nca");
    material::Cave.properties.diffuseTexture = asset::AcquireTextureAsset("cave/BaseColor.nca");
    material::Cave.properties.normalTexture = asset::AcquireTextureAsset("cave/Normal.nca");
}
} // namespace sample
