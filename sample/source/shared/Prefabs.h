#pragma once

#include "ncengine/asset/AssetViews.h"
#include "ncengine/asset/NcAsset.h"
#include "ncengine/graphics/Material.h"
#include "ncengine/graphics/PostProcess.h"

namespace nc::sample
{
// Load Asset Files
void InitializeResources();

// Reload cached global asset views
void ReloadPrefabs();

// Animations
namespace animation
{
extern asset::AssetId OgreIdle;
extern asset::AssetId OgreAttack;
extern asset::AssetId SkeletonIdle;
extern asset::AssetId SkeletonJump;
extern asset::AssetId SkeletonWalkRight;
extern asset::AssetId SkeletonWalkLeft;
extern asset::AssetId SkeletonWalkForward;
extern asset::AssetId SkeletonWalkBackward;
extern asset::AssetId StringLoose;
extern asset::AssetId StringModerate;
extern asset::AssetId StringStill;
extern asset::AssetId StringTight;
}

// Mesh
namespace mesh
{
constexpr auto CavePath = "cave.nca";
constexpr auto CreaturePath = "creature.nca";
constexpr auto CreatureArmPath = "creature_arm.nca";
constexpr auto Guy2Path = "guy2.nca";
constexpr auto HalfPipePath = "halfpipe.nca";
constexpr auto OgrePath = "ogre.nca";
constexpr auto RampPath = "ramp.nca";
constexpr auto SkeletonPath = "skeleton.nca";
constexpr auto StringPath = "string.nca";
constexpr auto TreePath = "tree.nca";

// Mesh
extern asset::MeshView Capsule;
extern asset::MeshView Cave;
extern asset::MeshView Creature;
extern asset::MeshView CreatureArm;
extern asset::MeshView Cube;
extern asset::MeshView Guy2;
extern asset::MeshView HalfPipe;
extern asset::MeshView Ogre;
extern asset::MeshView Plane;
extern asset::MeshView Ramp;
extern asset::MeshView Skeleton;
extern asset::MeshView Sphere;
extern asset::MeshView String;
extern asset::MeshView Tree;
extern asset::MeshView Wheel;
} // namespace mesh

// Materials
namespace material
{
extern MaterialDesc Blue;
extern MaterialDesc Cave;
extern MaterialDesc Creature;
extern MaterialDesc CreatureArm;
extern MaterialDesc Default;
extern MaterialDesc Green;
extern MaterialDesc Guy2;
extern MaterialDesc Ogre;
extern MaterialDesc Orange;
extern MaterialDesc Purple;
extern MaterialDesc Red;
extern MaterialDesc Skeleton;
extern MaterialDesc String;
extern MaterialDesc Teal;
extern MaterialDesc Tree;
extern MaterialDesc Yellow;
} // namespace material

// Colliders
constexpr auto HalfPipeConcaveCollider = "halfpipe.nca";
constexpr auto RampHullCollider = "ramp.nca";

// CubeMaps
namespace cubemap
{
constexpr auto NightSkyPath = "night_sky.nca";
} // namespace cubemap

// ConvexHulls
namespace convex_hull
{
constexpr auto RampPath = "ramp.nca";
extern asset::AssetId Ramp;
} // namespace collider

// MeshColliders
namespace mesh_collider
{
constexpr auto HalfpipePath = "halfpipe.nca";
extern asset::AssetId Halfpipe;
} // namespace mesh_collider

// Fonts
extern asset::FontInfo UIFont;

namespace post_process
{
extern OutlinePassProperties Outline;
extern NoisePassProperties Noise;
} // namespace post_process
} // namespace nc::sample
