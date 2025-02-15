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
asset::MeshView Capsule{};
asset::MeshView Cave{};
asset::MeshView Cube{};
asset::MeshView Guy{};
asset::MeshView Guy2{};
asset::MeshView HalfPipe{};
asset::MeshView Ogre{};
asset::MeshView Plane{};
asset::MeshView Ramp{};
asset::MeshView Skeleton{};
asset::MeshView Sphere{};
asset::MeshView Wheel{};
} // namespace mesh

namespace material
{
MaterialDesc Blue{"BlueMaterial"};
MaterialDesc Cave{"CaveMaterial"};
MaterialDesc Default{"DefaultMaterial"};
MaterialDesc Green{"GreenMaterial"};
MaterialDesc Guy{"GuyMaterial"};
MaterialDesc Guy2{"Guy2Material"};
MaterialDesc Ogre{"OgreMaterial"};
MaterialDesc Orange{"OrangeMaterial"};
MaterialDesc Purple{"PurpleMaterial"};
MaterialDesc Red{"RedMaterial"};
MaterialDesc Skeleton{"SkeletonMaterial"};
MaterialDesc Teal{"TealMaterial"};
MaterialDesc Yellow{"YellowMaterial"};
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

namespace mesh_collider
{
asset::AssetId Halfpipe{MakeAnimId(HalfpipePath)};
} // namespace mesh_collider

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
    LoadAssets(assetSettings.convexHullsPath, asset::AssetFlags::None, &asset::LoadConvexHullAssets);
    LoadAssets(assetSettings.cubeMapsPath, asset::AssetFlags::None, &asset::LoadCubeMapAssets);
    LoadAssets(assetSettings.meshesPath, asset::AssetFlags::None, &asset::LoadMeshAssets);
    LoadAssets(assetSettings.meshCollidersPath, asset::AssetFlags::None, &asset::LoadMeshColliderAssets);
    LoadAssets(assetSettings.skeletalAnimationsPath, asset::AssetFlags::None, &asset::LoadSkeletalAnimationAssets);
    LoadFont(UIFont);

    std::vector<std::string> textures
    {
        "cave/BaseColor.nca",
        "guy_base_color.nca",
        "guy_2_base_color.nca",
        "linear_hatch.nca",
        "ogre/BaseColor.nca",
        "skeleton/BaseColor.nca",
        "solid_color/Blue.nca",
        "solid_color/Green.nca",
        "solid_color/Orange.nca",
        "solid_color/Purple.nca",
        "solid_color/Red.nca",
        "solid_color/Teal.nca",
        "solid_color/Yellow.nca"
    };

    asset::LoadTextureAssets(textures, false, asset::AssetFlags::TextureTypeImage);

    std::vector<std::string> normalMaps
    {
        "ogre/Normal.nca",
        "skeleton/Normal.nca",
        "cave/Normal.nca",
        "guy_normal.nca",
        "guy_2_normal.nca",
    };

    asset::LoadTextureAssets(normalMaps, false, asset::AssetFlags::TextureTypeNormalMap);

    asset::LoadCubeMapAsset(cubemap::NightSkyPath);
}

void ReloadPrefabs()
{
    mesh::Cave = asset::AcquireMeshAsset(mesh::CavePath);
    mesh::Capsule = asset::AcquireMeshAsset(asset::CapsuleMesh);
    mesh::Cube = asset::AcquireMeshAsset(asset::CubeMesh);
    mesh::Guy = asset::AcquireMeshAsset(mesh::GuyPath);
    mesh::Guy2 = asset::AcquireMeshAsset(mesh::Guy2Path);
    mesh::HalfPipe = asset::AcquireMeshAsset(mesh::HalfPipePath);
    mesh::Ogre = asset::AcquireMeshAsset(mesh::OgrePath);
    mesh::Plane = asset::AcquireMeshAsset(asset::PlaneMesh);
    mesh::Ramp = asset::AcquireMeshAsset(mesh::RampPath);
    mesh::Skeleton = asset::AcquireMeshAsset(mesh::SkeletonPath);
    mesh::Sphere = asset::AcquireMeshAsset(asset::SphereMesh);
    mesh::Wheel = asset::AcquireMeshAsset(asset::WheelMesh);

    const auto normal = asset::AcquireTextureAsset(asset::DefaultNormal);
    material::Blue.properties.diffuseTex = asset::AcquireTextureAsset("solid_color/Blue.nca");
    material::Blue.properties.normalTex = normal;
    material::Blue.properties.normalIntensity = .220f;

    material::Cave.properties.diffuseTex = asset::AcquireTextureAsset("cave/BaseColor.nca");
    material::Cave.properties.normalTex = normal;
    material::Cave.properties.normalIntensity = .420f;

    material::Default.properties.diffuseTex = asset::AcquireTextureAsset(asset::DefaultBaseColor);
    material::Default.properties.normalTex = normal;

    material::Green.properties.diffuseTex = asset::AcquireTextureAsset("solid_color/Green.nca");
    material::Green.properties.normalTex = normal;
    material::Green.properties.normalIntensity = 4.0f;

    material::Guy.properties.diffuseTex = asset::AcquireTextureAsset("guy_base_color.nca");
    material::Guy.properties.normalTex = asset::AcquireTextureAsset("guy_normal.nca");;
    material::Guy.properties.hatchTex = asset::AcquireTextureAsset("linear_hatch.nca");;
    material::Guy.properties.normalIntensity = .39f;
    material::Guy.properties.hatchTiling = 12.0f;

    material::Guy2.properties.diffuseTex = asset::AcquireTextureAsset("guy_2_base_color.nca");
    material::Guy2.properties.normalTex = asset::AcquireTextureAsset("guy_2_normal.nca");;
    material::Guy2.properties.hatchTex = asset::AcquireTextureAsset("linear_hatch.nca");;
    material::Guy2.properties.normalIntensity = 0.09f;
    material::Guy2.properties.hatchTiling = 10.0f;

    material::Ogre.properties.diffuseTex = asset::AcquireTextureAsset("ogre/BaseColor.nca");
    material::Ogre.properties.normalTex = normal;
    material::Ogre.properties.normalIntensity = .280f;

    material::Orange.properties.diffuseTex = asset::AcquireTextureAsset("solid_color/Orange.nca");
    material::Orange.properties.normalTex = normal;
    material::Orange.properties.normalIntensity = .10f;

    material::Purple.properties.diffuseTex = asset::AcquireTextureAsset("solid_color/Purple.nca");
    material::Purple.properties.normalTex = normal;

    material::Red.properties.diffuseTex = asset::AcquireTextureAsset("solid_color/Red.nca");
    material::Red.properties.normalTex = normal;

    material::Skeleton.properties.diffuseTex = asset::AcquireTextureAsset("skeleton/BaseColor.nca");
    material::Skeleton.properties.normalTex = normal;
    material::Skeleton.properties.normalIntensity = .420f;

    material::Teal.properties.diffuseTex = asset::AcquireTextureAsset("solid_color/Teal.nca");
    material::Teal.properties.normalTex = normal;

    material::Yellow.properties.diffuseTex = asset::AcquireTextureAsset("solid_color/Yellow.nca");
    material::Yellow.properties.normalTex = normal;

}
} // namespace sample
