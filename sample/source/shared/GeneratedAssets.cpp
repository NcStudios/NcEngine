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
    std::string{path::DefaultAudioClip},
    std::string{path::drums},
    std::string{path::hit},
};

nc::asset::AudioClipView DefaultAudioClip{};
nc::asset::AudioClipView drums{};
nc::asset::AudioClipView hit{};

void Load()
{
    nc::asset::LoadAudioClipAssets(g_paths);
}

void Acquire()
{
    DefaultAudioClip = nc::asset::AcquireAudioClipAsset(path::DefaultAudioClip);
    drums = nc::asset::AcquireAudioClipAsset(path::drums);
    hit = nc::asset::AcquireAudioClipAsset(path::hit);
}

} // namespace audio_clip

namespace convex_hull
{
const auto g_paths = std::array{
    std::string{path::DefaultConvexHull},
    std::string{path::ramp},
};

nc::asset::AssetId DefaultConvexHull{nc::utility::Fnv1a(path::DefaultConvexHull)};
nc::asset::AssetId ramp{nc::utility::Fnv1a(path::ramp)};

void Load()
{
    nc::asset::LoadConvexHullAssets(g_paths);
}

} // namespace convex_hull

namespace cube_map
{
const auto g_paths = std::array{
    std::string{path::DefaultSkyboxCubeMap},
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
    std::string{path::DefaultPlane},
    std::string{path::DefaultCube},
    std::string{path::DefaultSphere},
    std::string{path::DefaultCapsule},
    std::string{path::DefaultWheel},
    std::string{path::DefaultSkyboxMesh},
    std::string{path::cave},
    std::string{path::guy2},
    std::string{path::halfpipe},
    std::string{path::ogre},
    std::string{path::ramp},
    std::string{path::skeleton},
};

nc::asset::MeshView DefaultPlane{};
nc::asset::MeshView DefaultCube{};
nc::asset::MeshView DefaultSphere{};
nc::asset::MeshView DefaultCapsule{};
nc::asset::MeshView DefaultWheel{};
nc::asset::MeshView DefaultSkyboxMesh{};
nc::asset::MeshView cave{};
nc::asset::MeshView guy2{};
nc::asset::MeshView halfpipe{};
nc::asset::MeshView ogre{};
nc::asset::MeshView ramp{};
nc::asset::MeshView skeleton{};

void Load()
{
    nc::asset::LoadMeshAssets(g_paths);
}

void Acquire()
{
    DefaultPlane = nc::asset::AcquireMeshAsset(path::DefaultPlane);
    DefaultCube = nc::asset::AcquireMeshAsset(path::DefaultCube);
    DefaultSphere = nc::asset::AcquireMeshAsset(path::DefaultSphere);
    DefaultCapsule = nc::asset::AcquireMeshAsset(path::DefaultCapsule);
    DefaultWheel = nc::asset::AcquireMeshAsset(path::DefaultWheel);
    DefaultSkyboxMesh = nc::asset::AcquireMeshAsset(path::DefaultSkyboxMesh);
    cave = nc::asset::AcquireMeshAsset(path::cave);
    guy2 = nc::asset::AcquireMeshAsset(path::guy2);
    halfpipe = nc::asset::AcquireMeshAsset(path::halfpipe);
    ogre = nc::asset::AcquireMeshAsset(path::ogre);
    ramp = nc::asset::AcquireMeshAsset(path::ramp);
    skeleton = nc::asset::AcquireMeshAsset(path::skeleton);
}

} // namespace mesh

namespace mesh_collider
{
const auto g_paths = std::array{
    std::string{path::DefaultMeshCollider},
    std::string{path::halfpipe},
};

nc::asset::AssetId DefaultMeshCollider{nc::utility::Fnv1a(path::DefaultMeshCollider)};
nc::asset::AssetId halfpipe{nc::utility::Fnv1a(path::halfpipe)};

void Load()
{
    nc::asset::LoadMeshColliderAssets(g_paths);
}

} // namespace mesh_collider

namespace animation
{
const auto g_paths = std::array{
    std::string{path::DefaultCubeAnimation},
    std::string{path::ogre_attack},
    std::string{path::ogre_idle},
    std::string{path::skeleton_idle},
    std::string{path::skeleton_jump},
    std::string{path::skeleton_walk_back},
    std::string{path::skeleton_walk_forward},
    std::string{path::skeleton_walk_left},
    std::string{path::skeleton_walk_right},
};

nc::asset::AssetId DefaultCubeAnimation{nc::utility::Fnv1a(path::DefaultCubeAnimation)};
nc::asset::AssetId ogre_attack{nc::utility::Fnv1a(path::ogre_attack)};
nc::asset::AssetId ogre_idle{nc::utility::Fnv1a(path::ogre_idle)};
nc::asset::AssetId skeleton_idle{nc::utility::Fnv1a(path::skeleton_idle)};
nc::asset::AssetId skeleton_jump{nc::utility::Fnv1a(path::skeleton_jump)};
nc::asset::AssetId skeleton_walk_back{nc::utility::Fnv1a(path::skeleton_walk_back)};
nc::asset::AssetId skeleton_walk_forward{nc::utility::Fnv1a(path::skeleton_walk_forward)};
nc::asset::AssetId skeleton_walk_left{nc::utility::Fnv1a(path::skeleton_walk_left)};
nc::asset::AssetId skeleton_walk_right{nc::utility::Fnv1a(path::skeleton_walk_right)};

void Load()
{
    nc::asset::LoadSkeletalAnimationAssets(g_paths);
}

} // namespace animation

namespace texture
{
nc::asset::TextureView DefaultBaseColor{};
nc::asset::TextureView DefaultMetallic{};
nc::asset::TextureView DefaultNormal{};
nc::asset::TextureView DefaultParticle{};
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
    std::string{path::DefaultBaseColor},
    std::string{path::DefaultMetallic},
    std::string{path::DefaultNormal},
    std::string{path::DefaultParticle},
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

const auto g_subtypes = std::array{
    nc::asset::AssetSubtype::DiffuseTexture,
    nc::asset::AssetSubtype::DiffuseTexture,
    nc::asset::AssetSubtype::NormalTexture,
    nc::asset::AssetSubtype::DiffuseTexture,
    nc::asset::AssetSubtype::DiffuseTexture,
    nc::asset::AssetSubtype::DiffuseTexture,
    nc::asset::AssetSubtype::DiffuseTexture,
    nc::asset::AssetSubtype::DiffuseTexture,
    nc::asset::AssetSubtype::DiffuseTexture,
    nc::asset::AssetSubtype::DiffuseTexture,
    nc::asset::AssetSubtype::DiffuseTexture,
    nc::asset::AssetSubtype::DiffuseTexture,
    nc::asset::AssetSubtype::DiffuseTexture,
    nc::asset::AssetSubtype::DiffuseTexture,
    nc::asset::AssetSubtype::DiffuseTexture,
    nc::asset::AssetSubtype::DiffuseTexture,
    nc::asset::AssetSubtype::DiffuseTexture,
    nc::asset::AssetSubtype::NormalTexture,
    nc::asset::AssetSubtype::NormalTexture,
    nc::asset::AssetSubtype::NormalTexture,
    nc::asset::AssetSubtype::NormalTexture,
};

void Load()
{
    nc::asset::LoadTextureAssets(g_paths, g_subtypes);
}

void Acquire()
{
    DefaultBaseColor = nc::asset::AcquireTextureAsset(path::DefaultBaseColor);
    DefaultMetallic = nc::asset::AcquireTextureAsset(path::DefaultMetallic);
    DefaultNormal = nc::asset::AcquireTextureAsset(path::DefaultNormal);
    DefaultParticle = nc::asset::AcquireTextureAsset(path::DefaultParticle);
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

} // namespace nc::sample

