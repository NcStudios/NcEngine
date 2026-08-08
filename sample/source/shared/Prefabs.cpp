#include "Prefabs.h"

#include "ncengine/asset/Assets.h"
#include "ncengine/config/Config.h"
#include "ncutility/Hash.h"

#include <filesystem>

namespace nc::sample
{
bool IsInitialized = false;

namespace font
{
asset::FontInfo ui{"SourceCodePro-Regular.ttf", 16.0f};
} // namespace font

namespace material
{
MaterialDesc blue{"BlueMaterial"};
MaterialDesc green{"GreenMaterial"};
MaterialDesc guy2{"Guy2Material"};
MaterialDesc ogre{"OgreMaterial"};
MaterialDesc tree{"TreeMaterial"};
MaterialDesc tree_leaves{"TreeLeaves"};
MaterialDesc orange{"OrangeMaterial"};
MaterialDesc purple{"PurpleMaterial"};
MaterialDesc red{"RedMaterial"};
MaterialDesc skeleton{"SkeletonMaterial"};
MaterialDesc teal{"TealMaterial"};
MaterialDesc white{"WhileMaterial"};
MaterialDesc yellow{"YellowMaterial"};
} // namespace material

namespace post_process
{
OutlinePassProperties outline{};
NoisePassProperties noise{};
} // namespace post_process

void InitializeResources()
{
    if (IsInitialized)
    {
        return;
    }

    IsInitialized = true;

    nc::asset::LoadFont(font::ui);

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
    audio_clip::Acquire();
    mesh::Acquire();
    texture::Acquire();

    auto materialDefaults = MaterialProperties
    {
        .gradientStart = Vector4::One(),
        .gradientEnd = Vector4::One(),
        .primaryColor = Vector4::One(),
        .secondaryColor = Vector4::One(),
        .tertiaryColor = Vector4::One(),
        .normalIntensity = 1.0f,
        .hatchTiling = 0.0f,
        .gradientAmount = 0.025f,
        .reflectivity = 0.0f,
        .diffuseTex = asset::AcquireTextureAsset(asset::DefaultBaseColor),
        .normalTex = asset::AcquireTextureAsset(asset::DefaultNormal),
        .hatchTex = asset::AcquireTextureAsset(asset::DefaultBaseColor),
        .useTextureNormals = 0,
        .useFlatShading = 0,
        .useColorOverride = 0,
        .useHatchTexture = 0
    };

    material::blue.properties = materialDefaults;
    material::blue.properties.diffuseTex = texture::diffuse_blue;
    material::blue.properties.normalIntensity = .220f;

    material::white.properties = materialDefaults;

    material::green.properties = materialDefaults;
    material::green.properties.diffuseTex = texture::diffuse_green;
    material::green.properties.normalIntensity = 4.0f;

    material::guy2.properties = materialDefaults;
    material::guy2.properties.diffuseTex = texture::diffuse_guy;
    material::guy2.properties.normalTex = texture::normal_guy;
    material::guy2.properties.hatchTex = texture::effect_linear_hatch;
    material::guy2.properties.normalIntensity = 5.0f;
    material::guy2.properties.hatchTiling = 16.0f;
    material::guy2.properties.reflectivity = 1.0f;
    material::guy2.properties.useTextureNormals = 1;
    material::guy2.properties.gradientStart = Vector4{.985f, .401f, .401f, 1.0f};
    material::guy2.properties.gradientEnd = Vector4{0.0f, 0.021f, 0.363f, 1.0f};
    material::guy2.properties.gradientAmount = 0.192f;
    material::guy2.properties.useFlatShading = 1;

    material::ogre.properties = materialDefaults;
    material::ogre.properties.diffuseTex = texture::diffuse_ogre;
    material::ogre.properties.normalIntensity = .280f;
    material::ogre.properties.useFlatShading = 1;

    material::orange.properties = materialDefaults;
    material::orange.properties.diffuseTex = texture::diffuse_orange;
    material::orange.properties.normalIntensity = .10f;
    material::orange.properties.useTextureNormals = 1;

    material::purple.properties = materialDefaults;
    material::purple.properties.diffuseTex = texture::diffuse_purple;

    material::red.properties = materialDefaults;
    material::red.properties.diffuseTex = texture::diffuse_red;

    material::skeleton.properties = materialDefaults;
    material::skeleton.properties.diffuseTex = texture::diffuse_skeleton;
    material::skeleton.properties.normalIntensity = .420f;
    material::skeleton.properties.useFlatShading = 1;

    material::teal.properties = materialDefaults;
    material::teal.properties.diffuseTex = texture::diffuse_teal;

    material::yellow.properties = materialDefaults;
    material::yellow.properties.diffuseTex = texture::diffuse_yellow;

    material::tree.properties = materialDefaults;
    material::tree.properties.diffuseTex = texture::diffuse_tree;
    material::tree.properties.diffuseTex = texture::normal_tree;

    material::tree_leaves.properties = materialDefaults;
    material::tree_leaves.properties.diffuseTex = texture::diffuse_tree_leaves;

    post_process::outline = OutlinePassProperties
    {
        .color = Vector3{0.0f, 0.0f, 0.0f},
        .width = 1.0f,
        .depthThreshold = 3.6f,
        .viewDirDepthThreshold = 0.04f,
        .normalThreshold = 0.940f
    };

    post_process::noise = NoisePassProperties
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
