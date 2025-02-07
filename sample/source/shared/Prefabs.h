#pragma once

#include "ncengine/asset/AssetViews.h"
#include "ncengine/asset/NcAsset.h"
#include "ncengine/graphics/Material.h"

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
}

// Mesh
namespace mesh
{
constexpr auto GuyPath = "guy.nca";
constexpr auto HalfPipePath = "halfpipe.nca";
constexpr auto RampPath = "ramp.nca";
constexpr auto OgrePath = "ogre.nca";
constexpr auto SkeletonPath = "skeleton.nca";
constexpr auto CavePath = "cave.nca";

// Mesh
extern asset::MeshView Cube;
extern asset::MeshView Sphere;
extern asset::MeshView Capsule;
extern asset::MeshView Guy;
extern asset::MeshView Plane;
extern asset::MeshView Wheel;
extern asset::MeshView HalfPipe;
extern asset::MeshView Ramp;
extern asset::MeshView Ogre;
extern asset::MeshView Skeleton;
extern asset::MeshView Cave;
} // namespace mesh

// Materials
namespace material
{
extern MaterialDesc Blue;
extern MaterialDesc Cave;
extern MaterialDesc Default;
extern MaterialDesc Green;
extern MaterialDesc Guy;
extern MaterialDesc Ogre;
extern MaterialDesc Orange;
extern MaterialDesc Purple;
extern MaterialDesc Red;
extern MaterialDesc Skeleton;
extern MaterialDesc Teal;
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
} // namespace nc::sample
