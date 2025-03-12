#include "Prefabs.h"

#include "ncengine/asset/Assets.h"
#include "ncengine/asset/DefaultAssets.h"
#include "ncengine/config/Config.h"
#include "ncutility/Hash.h"

#include <filesystem>

namespace nc::sample
{
bool IsInitialized = false;

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

namespace mesh
{
asset::MeshView Cube{};
asset::MeshView Sphere{};
asset::MeshView Capsule{};
asset::MeshView Plane{};
asset::MeshView Wheel{};
} // namespace mesh


asset::FontInfo UIFont{"SourceCodePro-Regular.ttf", 16.0f};

namespace post_process
{
OutlinePassProperties Outline{};
NoisePassProperties Noise{};
} // namespace post_process

void InitializeResources()
{
    if (IsInitialized)
    {
        return;
    }

    IsInitialized = true;

    nc::asset::LoadFont(UIFont);

    // TODO: load default assets...

    audio_clip::Load();
    convex_hull::Load();
    cube_map::Load();
    mesh::Load();
    mesh_collider::Load();
    animation::Load();
    texture::Load();
}

void ReloadPrefabs()
{


    mesh::Capsule = asset::AcquireMeshAsset(asset::CapsuleMesh);
    mesh::Cube = asset::AcquireMeshAsset(asset::CubeMesh);
    mesh::Plane = asset::AcquireMeshAsset(asset::PlaneMesh);
    mesh::Sphere = asset::AcquireMeshAsset(asset::SphereMesh);
    mesh::Wheel = asset::AcquireMeshAsset(asset::WheelMesh);

    mesh::Acquire();
    texture::Acquire();

    auto materialDefaults = MaterialProperties
    {
        .gradientStart = Vector3{1.0f, 1.0f, 1.0f},
        .diffuseTex = asset::AcquireTextureAsset(asset::DefaultBaseColor),
        .gradientEnd = Vector3{1.0f, 1.0f, 1.0f},
        .normalTex = asset::AcquireTextureAsset(asset::DefaultNormal),
        .hatchTex = asset::AcquireTextureAsset(asset::DefaultBaseColor),
        .normalIntensity = 1.0f,
        .hatchTiling = 0.0f,
        .gradientAmount = 0.025f,
        .reflectivity = 0.0f,
        .useTextureNormals = 0,
        .useFlatShading = 0
    };

    material::Blue.properties = materialDefaults;
    material::Blue.properties.diffuseTex = texture::diffuse_blue;
    material::Blue.properties.normalIntensity = .220f;

    material::Cave.properties = materialDefaults;
    material::Cave.properties.diffuseTex = texture::diffuse_cave;
    material::Cave.properties.normalIntensity = .420f;
    material::Cave.properties.useFlatShading = 1;

    material::Default.properties = materialDefaults;

    material::Green.properties = materialDefaults;
    material::Green.properties.diffuseTex = texture::diffuse_green;
    material::Green.properties.normalIntensity = 4.0f;

    material::Guy2.properties = materialDefaults;
    material::Guy2.properties.diffuseTex = texture::diffuse_guy;
    material::Guy2.properties.normalTex = texture::normal_guy;
    material::Guy2.properties.hatchTex = texture::effect_linear_hatch;
    material::Guy2.properties.normalIntensity = 5.0f;
    material::Guy2.properties.hatchTiling = 16.0f;
    material::Guy2.properties.reflectivity = 1.0f;
    material::Guy2.properties.useTextureNormals = 1;
    material::Guy2.properties.gradientStart = Vector3{.985f, .401f, .401f};
    material::Guy2.properties.gradientEnd = Vector3{0.0f, 0.021f, 0.363f};
    material::Guy2.properties.gradientAmount = 0.192f;
    material::Guy2.properties.useFlatShading = 1;

    material::Ogre.properties = materialDefaults;
    material::Ogre.properties.diffuseTex = texture::diffuse_ogre;
    material::Ogre.properties.normalIntensity = .280f;
    material::Ogre.properties.useFlatShading = 1;

    material::Orange.properties = materialDefaults;
    material::Orange.properties.diffuseTex = texture::diffuse_orange;
    material::Orange.properties.normalIntensity = .10f;
    material::Orange.properties.useTextureNormals = 1;

    material::Purple.properties = materialDefaults;
    material::Purple.properties.diffuseTex = texture::diffuse_purple;

    material::Red.properties = materialDefaults;
    material::Red.properties.diffuseTex = texture::diffuse_red;

    material::Skeleton.properties = materialDefaults;
    material::Skeleton.properties.diffuseTex = texture::diffuse_skeleton;
    material::Skeleton.properties.normalIntensity = .420f;
    material::Skeleton.properties.useFlatShading = 1;

    material::Teal.properties = materialDefaults;
    material::Teal.properties.diffuseTex = texture::diffuse_teal;

    material::Yellow.properties = materialDefaults;
    material::Yellow.properties.diffuseTex = texture::diffuse_yellow;

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
        .noiseTex = texture::effect_noise,
        .noiseTexAmount = 0.1f,
        .noiseTexTiling = 1.0f,
    };
}
} // namespace sample
