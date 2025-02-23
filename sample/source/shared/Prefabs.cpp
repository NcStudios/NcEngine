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

namespace post_process
{
OutlinePassProperties Outline{};
NoisePassProperties Noise{};
} // namespace post_process

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
        "guy_2_base_color.nca",
        "halftone.nca",
        "linear_hatch.nca",
        "noise.nca",
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
        "guy_2_normal.nca"
    };

    asset::LoadTextureAssets(normalMaps, false, asset::AssetFlags::TextureTypeNormalMap);

    asset::LoadCubeMapAsset(cubemap::NightSkyPath);

}

void ReloadPrefabs()
{
    mesh::Cave = asset::AcquireMeshAsset(mesh::CavePath);
    mesh::Capsule = asset::AcquireMeshAsset(asset::CapsuleMesh);
    mesh::Cube = asset::AcquireMeshAsset(asset::CubeMesh);
    mesh::Guy2 = asset::AcquireMeshAsset(mesh::Guy2Path);
    mesh::HalfPipe = asset::AcquireMeshAsset(mesh::HalfPipePath);
    mesh::Ogre = asset::AcquireMeshAsset(mesh::OgrePath);
    mesh::Plane = asset::AcquireMeshAsset(asset::PlaneMesh);
    mesh::Ramp = asset::AcquireMeshAsset(mesh::RampPath);
    mesh::Skeleton = asset::AcquireMeshAsset(mesh::SkeletonPath);
    mesh::Sphere = asset::AcquireMeshAsset(asset::SphereMesh);
    mesh::Wheel = asset::AcquireMeshAsset(asset::WheelMesh);

    auto materialDefaults = MaterialProperties
    {
        .gradientStart = Vector3{0.5f, 0.5f, 1.0f},
        .diffuseTex = asset::AcquireTextureAsset(asset::DefaultBaseColor),
        .gradientEnd = Vector3{1.0f, 0.5f, 0.5f},
        .normalTex = asset::AcquireTextureAsset(asset::DefaultNormal),
        .hatchTex = asset::AcquireTextureAsset(asset::DefaultBaseColor),
        .normalIntensity = 1.0f,
        .hatchTiling = 0.0f,
        .gradientAmount = 0.025f,
        .reflectivity = 0.0f,
        .useTextureNormals = 0
    };

    material::Blue.properties = materialDefaults;
    material::Blue.properties.diffuseTex = asset::AcquireTextureAsset("solid_color/Blue.nca");
    material::Blue.properties.normalIntensity = .220f;

    material::Cave.properties = materialDefaults;
    material::Cave.properties.diffuseTex = asset::AcquireTextureAsset("cave/BaseColor.nca");
    material::Cave.properties.normalIntensity = .420f;

    material::Default.properties = materialDefaults;

    material::Green.properties = materialDefaults;
    material::Green.properties.diffuseTex = asset::AcquireTextureAsset("solid_color/Green.nca");
    material::Green.properties.normalIntensity = 4.0f;

    material::Guy2.properties = materialDefaults;
    material::Guy2.properties.diffuseTex = asset::AcquireTextureAsset("guy_2_base_color.nca");
    material::Guy2.properties.normalTex = asset::AcquireTextureAsset("guy_2_normal.nca");
    material::Guy2.properties.hatchTex = asset::AcquireTextureAsset("linear_hatch.nca");
    material::Guy2.properties.normalIntensity = 0.0f;
    material::Guy2.properties.hatchTiling = 16.0f;
    material::Guy2.properties.reflectivity = 1.0f;
    material::Guy2.properties.useTextureNormals = 1;
    material::Guy2.properties.gradientStart = Vector3{0.597f, 0.79f, 1.0f};
    material::Guy2.properties.gradientEnd = Vector3{0.96f, 0.46f, 1.0f};
    material::Guy2.properties.gradientAmount = 0.038f;

    material::Ogre.properties = materialDefaults;
    material::Ogre.properties.diffuseTex = asset::AcquireTextureAsset("ogre/BaseColor.nca");
    material::Ogre.properties.normalIntensity = .280f;

    material::Orange.properties = materialDefaults;
    material::Orange.properties.diffuseTex = asset::AcquireTextureAsset("solid_color/Orange.nca");
    material::Orange.properties.normalIntensity = .10f;
    material::Orange.properties.useTextureNormals = 1;

    material::Purple.properties = materialDefaults;
    material::Purple.properties.diffuseTex = asset::AcquireTextureAsset("solid_color/Purple.nca");

    material::Red.properties = materialDefaults;
    material::Red.properties.diffuseTex = asset::AcquireTextureAsset("solid_color/Red.nca");

    material::Skeleton.properties = materialDefaults;
    material::Skeleton.properties.diffuseTex = asset::AcquireTextureAsset("skeleton/BaseColor.nca");
    material::Skeleton.properties.normalIntensity = .420f;

    material::Teal.properties = materialDefaults;
    material::Teal.properties.diffuseTex = asset::AcquireTextureAsset("solid_color/Teal.nca");

    material::Yellow.properties = materialDefaults;
    material::Yellow.properties.diffuseTex = asset::AcquireTextureAsset("solid_color/Yellow.nca");

    post_process::Outline = OutlinePassProperties
    {
        .color = Vector3{0.0f, 0.0f, 0.0f},
        .width = 1.0f,
        .depthThreshold = 3.6f,
        .viewDirDepthThreshold = 0.04f,
        .normalThreshold = 0.940f
    };

    post_process::Noise = NoisePassProperties
    {
        .maskGradientStart = Vector3{1.0f, 1.0f, 1.0f},
        .maskGradientAmount = 1.0f,
        .maskGradientEnd = Vector3{0.0f, 0.0f, 0.0f},
        .noiseTex = asset::AcquireTextureAsset("noise.nca"),
        .noiseTexAmount = 0.1f,
        .noiseTexTiling = 1.0f,
    };
}
} // namespace sample
