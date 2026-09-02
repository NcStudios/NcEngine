/** @note This file is auto generated. */

#pragma once

#include "ncengine/asset/AssetViews.h"

namespace nc::sample
{
namespace audio_clip
{
namespace path
{
constexpr auto default_silence = "default/silence.nca";
constexpr auto drums = "drums.nca";
constexpr auto hit = "hit.nca";
} // namespace path

extern nc::asset::AudioClipView default_silence;
extern nc::asset::AudioClipView drums;
extern nc::asset::AudioClipView hit;

void Load();
void Acquire();
} // namespace audio_clip

namespace convex_hull
{
namespace path
{
constexpr auto default_cube = "default/cube.nca";
constexpr auto ramp = "ramp.nca";
} // namespace path

extern nc::asset::AssetId default_cube;
extern nc::asset::AssetId ramp;

void Load();
} // namespace convex_hull

namespace cube_map
{
namespace path
{
constexpr auto default_skybox = "default/skybox.nca";
constexpr auto night_sky = "night_sky.nca";
} // namespace path

void Load();
} // namespace cube_map

namespace mesh
{
namespace path
{
constexpr auto default_plane = "default/plane.nca";
constexpr auto default_cube = "default/cube.nca";
constexpr auto default_sphere = "default/sphere.nca";
constexpr auto default_capsule = "default/capsule.nca";
constexpr auto default_wheel = "default/wheel.nca";
constexpr auto default_skybox = "default/skybox.nca";
constexpr auto guy2 = "guy2.nca";
constexpr auto halfpipe = "halfpipe.nca";
constexpr auto ogre = "ogre.nca";
constexpr auto ramp = "ramp.nca";
constexpr auto skeleton = "skeleton.nca";
constexpr auto sword = "sword.nca";
} // namespace path

extern nc::asset::MeshView default_plane;
extern nc::asset::MeshView default_cube;
extern nc::asset::MeshView default_sphere;
extern nc::asset::MeshView default_capsule;
extern nc::asset::MeshView default_wheel;
extern nc::asset::MeshView default_skybox;
extern nc::asset::MeshView guy2;
extern nc::asset::MeshView halfpipe;
extern nc::asset::MeshView ogre;
extern nc::asset::MeshView ramp;
extern nc::asset::MeshView skeleton;
extern nc::asset::MeshView sword;

void Load();
void Acquire();
} // namespace mesh

namespace mesh_collider
{
namespace path
{
constexpr auto default_plane = "default/plane.nca";
constexpr auto halfpipe = "halfpipe.nca";
} // namespace path

extern nc::asset::AssetId default_plane;
extern nc::asset::AssetId halfpipe;

void Load();
} // namespace mesh_collider

namespace animation
{
namespace path
{
constexpr auto default_cube = "default/cube.nca";
constexpr auto ogre_attack = "ogre/attack.nca";
constexpr auto ogre_idle = "ogre/idle.nca";
constexpr auto skeleton_idle = "skeleton/idle.nca";
constexpr auto skeleton_jump = "skeleton/jump.nca";
constexpr auto skeleton_walk_back = "skeleton/walk_back.nca";
constexpr auto skeleton_walk_forward = "skeleton/walk_forward.nca";
constexpr auto skeleton_walk_left = "skeleton/walk_left.nca";
constexpr auto skeleton_walk_right = "skeleton/walk_right.nca";
} // namespace path

extern nc::asset::AssetId default_cube;
extern nc::asset::AssetId ogre_attack;
extern nc::asset::AssetId ogre_idle;
extern nc::asset::AssetId skeleton_idle;
extern nc::asset::AssetId skeleton_jump;
extern nc::asset::AssetId skeleton_walk_back;
extern nc::asset::AssetId skeleton_walk_forward;
extern nc::asset::AssetId skeleton_walk_left;
extern nc::asset::AssetId skeleton_walk_right;

void Load();
} // namespace animation

namespace texture
{
namespace path
{
constexpr auto default_color = "default/color.nca";
constexpr auto default_normal = "default/normal.nca";
constexpr auto default_particle = "default/particle.nca";
constexpr auto diffuse_guy = "diffuse/guy.nca";
constexpr auto diffuse_ogre = "diffuse/ogre.nca";
constexpr auto diffuse_skeleton = "diffuse/skeleton.nca";
constexpr auto diffuse_blue = "diffuse/blue.nca";
constexpr auto diffuse_green = "diffuse/green.nca";
constexpr auto diffuse_orange = "diffuse/orange.nca";
constexpr auto diffuse_purple = "diffuse/purple.nca";
constexpr auto diffuse_red = "diffuse/red.nca";
constexpr auto diffuse_teal = "diffuse/teal.nca";
constexpr auto diffuse_yellow = "diffuse/yellow.nca";
constexpr auto diffuse_sword = "diffuse/sword.nca";
constexpr auto effect_linear_hatch = "effect/linear_hatch.nca";
constexpr auto effect_noise = "effect/noise.nca";
constexpr auto normal_guy = "normal/guy.nca";
constexpr auto normal_ogre = "normal/ogre.nca";
constexpr auto normal_skeleton = "normal/skeleton.nca";
constexpr auto normal_sword = "normal/sword.nca";
} // namespace path

extern nc::asset::TextureView default_color;
extern nc::asset::TextureView default_normal;
extern nc::asset::TextureView default_particle;
extern nc::asset::TextureView diffuse_guy;
extern nc::asset::TextureView diffuse_ogre;
extern nc::asset::TextureView diffuse_skeleton;
extern nc::asset::TextureView diffuse_blue;
extern nc::asset::TextureView diffuse_green;
extern nc::asset::TextureView diffuse_orange;
extern nc::asset::TextureView diffuse_purple;
extern nc::asset::TextureView diffuse_red;
extern nc::asset::TextureView diffuse_teal;
extern nc::asset::TextureView diffuse_yellow;
extern nc::asset::TextureView diffuse_sword;
extern nc::asset::TextureView effect_linear_hatch;
extern nc::asset::TextureView effect_noise;
extern nc::asset::TextureView normal_guy;
extern nc::asset::TextureView normal_ogre;
extern nc::asset::TextureView normal_skeleton;
extern nc::asset::TextureView normal_sword;

void Load();
void Acquire();
} // namespace texture

} // namespace nc::sample

