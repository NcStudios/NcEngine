/** @note This file is auto generated. */

#include "GeneratedAssets.h"

#include "ncengine/asset/Assets.h"
#include "ncutility/Hash.h"

#include <array>
#include <string>

namespace nc::sample
{
namespace audio_clip
{
const auto g_paths = std::array{
    std::string{path::default_silence},
    std::string{path::drums},
    std::string{path::hit},
};

nc::asset::AudioClipView default_silence{};
nc::asset::AudioClipView drums{};
nc::asset::AudioClipView hit{};

void Load()
{
    nc::asset::LoadAudioClipAssets(g_paths);
}

void Acquire()
{
    default_silence = nc::asset::AcquireAudioClipAsset(path::default_silence);
    drums = nc::asset::AcquireAudioClipAsset(path::drums);
    hit = nc::asset::AcquireAudioClipAsset(path::hit);
}

} // namespace audio_clip

namespace texture
{
nc::asset::TextureView default_color{};
nc::asset::TextureView default_normal{};
nc::asset::TextureView default_particle{};
nc::asset::TextureView diffuse_cave{};
nc::asset::TextureView diffuse_guy{};
nc::asset::TextureView diffuse_ogre{};
nc::asset::TextureView diffuse_skeleton{};
nc::asset::TextureView diffuse_blue{};
nc::asset::TextureView diffuse_green{};
nc::asset::TextureView diffuse_orange{};
nc::asset::TextureView diffuse_purple{};
nc::asset::TextureView diffuse_red{};
nc::asset::TextureView diffuse_teal{};
nc::asset::TextureView diffuse_yellow{};
nc::asset::TextureView effect_linear_hatch{};
nc::asset::TextureView effect_noise{};
nc::asset::TextureView normal_cave{};
nc::asset::TextureView normal_guy{};
nc::asset::TextureView normal_ogre{};
nc::asset::TextureView normal_skeleton{};

const auto g_paths = std::array{
    std::string{path::default_color},
    std::string{path::default_normal},
    std::string{path::default_particle},
    std::string{path::diffuse_cave},
    std::string{path::diffuse_guy},
    std::string{path::diffuse_ogre},
    std::string{path::diffuse_skeleton},
    std::string{path::diffuse_blue},
    std::string{path::diffuse_green},
    std::string{path::diffuse_orange},
    std::string{path::diffuse_purple},
    std::string{path::diffuse_red},
    std::string{path::diffuse_teal},
    std::string{path::diffuse_yellow},
    std::string{path::effect_linear_hatch},
    std::string{path::effect_noise},
    std::string{path::normal_cave},
    std::string{path::normal_guy},
    std::string{path::normal_ogre},
    std::string{path::normal_skeleton},
};

void Load()
{
    nc::asset::LoadTextureAssets(g_paths);
}

void Acquire()
{
    default_color = nc::asset::AcquireTextureAsset(path::default_color);
    default_normal = nc::asset::AcquireTextureAsset(path::default_normal);
    default_particle = nc::asset::AcquireTextureAsset(path::default_particle);
    diffuse_cave = nc::asset::AcquireTextureAsset(path::diffuse_cave);
    diffuse_guy = nc::asset::AcquireTextureAsset(path::diffuse_guy);
    diffuse_ogre = nc::asset::AcquireTextureAsset(path::diffuse_ogre);
    diffuse_skeleton = nc::asset::AcquireTextureAsset(path::diffuse_skeleton);
    diffuse_blue = nc::asset::AcquireTextureAsset(path::diffuse_blue);
    diffuse_green = nc::asset::AcquireTextureAsset(path::diffuse_green);
    diffuse_orange = nc::asset::AcquireTextureAsset(path::diffuse_orange);
    diffuse_purple = nc::asset::AcquireTextureAsset(path::diffuse_purple);
    diffuse_red = nc::asset::AcquireTextureAsset(path::diffuse_red);
    diffuse_teal = nc::asset::AcquireTextureAsset(path::diffuse_teal);
    diffuse_yellow = nc::asset::AcquireTextureAsset(path::diffuse_yellow);
    effect_linear_hatch = nc::asset::AcquireTextureAsset(path::effect_linear_hatch);
    effect_noise = nc::asset::AcquireTextureAsset(path::effect_noise);
    normal_cave = nc::asset::AcquireTextureAsset(path::normal_cave);
    normal_guy = nc::asset::AcquireTextureAsset(path::normal_guy);
    normal_ogre = nc::asset::AcquireTextureAsset(path::normal_ogre);
    normal_skeleton = nc::asset::AcquireTextureAsset(path::normal_skeleton);
}

} // namespace texture

namespace convex_hull
{
const auto g_paths = std::array{
    std::string{path::default_cube},
    std::string{path::ramp},
};

nc::asset::AssetId default_cube{nc::utility::Fnv1a(path::default_cube)};
nc::asset::AssetId ramp{nc::utility::Fnv1a(path::ramp)};

void Load()
{
    nc::asset::LoadConvexHullAssets(g_paths);
}

} // namespace convex_hull

namespace cube_map
{
const auto g_paths = std::array{
    std::string{path::default_skybox},
    std::string{path::night_sky},
};

void Load()
{
    nc::asset::LoadCubeMapAssets(g_paths);
}

} // namespace cube_map

namespace mesh
{
const auto g_paths = std::array{
    std::string{path::default_plane},
    std::string{path::default_cube},
    std::string{path::default_sphere},
    std::string{path::default_capsule},
    std::string{path::default_wheel},
    std::string{path::default_skybox},
    std::string{path::cave},
    std::string{path::guy2},
    std::string{path::halfpipe},
    std::string{path::ogre},
    std::string{path::ramp},
    std::string{path::skeleton},
    std::string{path::girl_dress},
    std::string{path::girl_body},
    std::string{path::cube},
};

nc::asset::MeshView default_plane{};
nc::asset::MeshView default_cube{};
nc::asset::MeshView default_sphere{};
nc::asset::MeshView default_capsule{};
nc::asset::MeshView default_wheel{};
nc::asset::MeshView default_skybox{};
nc::asset::MeshView cave{};
nc::asset::MeshView guy2{};
nc::asset::MeshView halfpipe{};
nc::asset::MeshView ogre{};
nc::asset::MeshView ramp{};
nc::asset::MeshView skeleton{};
nc::asset::MeshView girl_dress{};
nc::asset::MeshView girl_body{};
nc::asset::MeshView cube{};

void Load()
{
    nc::asset::LoadMeshAssets(g_paths);
}

void Acquire()
{
    default_plane = nc::asset::AcquireMeshAsset(path::default_plane);
    default_cube = nc::asset::AcquireMeshAsset(path::default_cube);
    default_sphere = nc::asset::AcquireMeshAsset(path::default_sphere);
    default_capsule = nc::asset::AcquireMeshAsset(path::default_capsule);
    default_wheel = nc::asset::AcquireMeshAsset(path::default_wheel);
    default_skybox = nc::asset::AcquireMeshAsset(path::default_skybox);
    cave = nc::asset::AcquireMeshAsset(path::cave);
    guy2 = nc::asset::AcquireMeshAsset(path::guy2);
    halfpipe = nc::asset::AcquireMeshAsset(path::halfpipe);
    ogre = nc::asset::AcquireMeshAsset(path::ogre);
    ramp = nc::asset::AcquireMeshAsset(path::ramp);
    skeleton = nc::asset::AcquireMeshAsset(path::skeleton);
    girl_dress = nc::asset::AcquireMeshAsset(path::girl_dress);
    girl_body = nc::asset::AcquireMeshAsset(path::girl_body);
    cube = nc::asset::AcquireMeshAsset(path::cube);
}

} // namespace mesh

namespace mesh_collider
{
const auto g_paths = std::array{
    std::string{path::default_plane},
    std::string{path::halfpipe},
};

nc::asset::AssetId default_plane{nc::utility::Fnv1a(path::default_plane)};
nc::asset::AssetId halfpipe{nc::utility::Fnv1a(path::halfpipe)};

void Load()
{
    nc::asset::LoadMeshColliderAssets(g_paths);
}

} // namespace mesh_collider

namespace animation
{
const auto g_paths = std::array{
    std::string{path::default_cube},
    std::string{path::ogre_attack},
    std::string{path::ogre_idle},
    std::string{path::skeleton_idle},
    std::string{path::skeleton_jump},
    std::string{path::skeleton_walk_back},
    std::string{path::skeleton_walk_forward},
    std::string{path::skeleton_walk_left},
    std::string{path::skeleton_walk_right},
    std::string{path::girl_twirl},
};

nc::asset::AssetId default_cube{nc::utility::Fnv1a(path::default_cube)};
nc::asset::AssetId ogre_attack{nc::utility::Fnv1a(path::ogre_attack)};
nc::asset::AssetId ogre_idle{nc::utility::Fnv1a(path::ogre_idle)};
nc::asset::AssetId skeleton_idle{nc::utility::Fnv1a(path::skeleton_idle)};
nc::asset::AssetId skeleton_jump{nc::utility::Fnv1a(path::skeleton_jump)};
nc::asset::AssetId skeleton_walk_back{nc::utility::Fnv1a(path::skeleton_walk_back)};
nc::asset::AssetId skeleton_walk_forward{nc::utility::Fnv1a(path::skeleton_walk_forward)};
nc::asset::AssetId skeleton_walk_left{nc::utility::Fnv1a(path::skeleton_walk_left)};
nc::asset::AssetId skeleton_walk_right{nc::utility::Fnv1a(path::skeleton_walk_right)};
nc::asset::AssetId girl_twirl{nc::utility::Fnv1a(path::girl_twirl)};

void Load()
{
    nc::asset::LoadSkeletalAnimationAssets(g_paths);
}

} // namespace animation

} // namespace nc::sample

