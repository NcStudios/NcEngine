#include "Prefabs.h"

#include "ncengine/asset/Assets.h"
#include "ncengine/asset/DefaultAssets.h"
#include "ncengine/config/Config.h"
#include "ncutility/Hash.h"

#include <filesystem>

namespace nc::sample
{
bool IsInitialized = false;

auto MakeAnimId(std::string_view path) -> uint64_t
{
    return utility::Fnv1a(std::filesystem::path(path).make_preferred().string());
}

namespace mesh
{
asset::MeshView Cube{};
asset::MeshView Sphere{};
asset::MeshView Capsule{};
asset::MeshView Plane{};
asset::MeshView Wheel{};
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
asset::AssetId OgreIdle{MakeAnimId("ogre/idle.nca")};
asset::AssetId OgreAttack{MakeAnimId("ogre/attack.nca")};
asset::AssetId SkeletonIdle{MakeAnimId("skeleton/idle.nca")};
asset::AssetId SkeletonJump{MakeAnimId("skeleton/jump.nca")};
asset::AssetId SkeletonWalkRight{MakeAnimId("skeleton/walk_right.nca")};
asset::AssetId SkeletonWalkLeft{MakeAnimId("skeleton/walk_left.nca")};
asset::AssetId SkeletonWalkForward{MakeAnimId("skeleton/walk_forward.nca")};
asset::AssetId SkeletonWalkBackward{MakeAnimId("skeleton/walk_back.nca")};
} // namespace animation

namespace convex_hull
{
asset::AssetId Ramp{MakeAnimId(RampPath)};
} // namespace convex_hull

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
    mesh::Wheel = asset::AcquireMeshAsset(asset::WheelMesh);
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
    material::Green.properties.normalIntensity = 4.0f;
    material::Blue.properties.diffuseTexture = asset::AcquireTextureAsset("solid_color/Blue.nca");
    material::Blue.properties.normalTexture = normal;
    material::Blue.properties.normalIntensity = .220f;
    material::Orange.properties.diffuseTexture = asset::AcquireTextureAsset("solid_color/Orange.nca");
    material::Orange.properties.normalTexture = normal;
    material::Orange.properties.normalIntensity = .10f;
    material::Purple.properties.diffuseTexture = asset::AcquireTextureAsset("solid_color/Purple.nca");
    material::Purple.properties.normalTexture = normal;
    material::Teal.properties.diffuseTexture = asset::AcquireTextureAsset("solid_color/Teal.nca");
    material::Teal.properties.normalTexture = normal;
    material::Yellow.properties.diffuseTexture = asset::AcquireTextureAsset("solid_color/Yellow.nca");
    material::Yellow.properties.normalTexture = normal;
    material::Ogre.properties.diffuseTexture = asset::AcquireTextureAsset("ogre/BaseColor.nca");
    material::Ogre.properties.normalTexture = asset::AcquireTextureAsset("ogre/Normal.nca");
    material::Ogre.properties.normalIntensity = .280f;
    material::Skeleton.properties.diffuseTexture = asset::AcquireTextureAsset("skeleton/BaseColor.nca");
    material::Skeleton.properties.normalTexture = asset::AcquireTextureAsset("skeleton/Normal.nca");
    material::Skeleton.properties.normalIntensity = .420f;
    material::Cave.properties.diffuseTexture = asset::AcquireTextureAsset("cave/BaseColor.nca");
    material::Cave.properties.normalTexture = asset::AcquireTextureAsset("cave/Normal.nca");
    material::Cave.properties.normalIntensity = .420f;
}
} // namespace sample
