#pragma once

#include "ncengine/asset/AssetViews.h"
#include "ncengine/asset/NcAsset.h"
#include "ncengine/graphics/Material.h"

namespace nc::sample
{
// Load Asset Files
void InitializeResources();

// Prefabs
void ReloadPrefabs();

// Animations
namespace animation
{
extern uint64_t OgreAttack;
extern uint64_t OgreIdle;
extern uint64_t SkeletonIdle;
extern uint64_t SkeletonJump;
extern uint64_t SkeletonWalkBackward;
extern uint64_t SkeletonWalkForward;
extern uint64_t SkeletonWalkLeft;
extern uint64_t SkeletonWalkRight;
} // namespace animation

// Colliders
constexpr auto HalfPipeConcaveCollider = "halfpipe.nca";
constexpr auto RampHullCollider = "ramp.nca";

// CubeMaps
namespace cubemap
{
constexpr auto NightSkyPath = "night_sky.nca";
} // namespace cubemap

// Fonts
extern asset::FontInfo UIFont;

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

// Mesh
constexpr auto HalfPipeMesh = "halfpipe.nca";
constexpr auto RampMesh = "ramp.nca";

namespace mesh
{
constexpr auto CavePath = "cave.nca";
constexpr auto GuyPath = "guy.nca";
constexpr auto HalfPipePath = "halfpipe.nca";
constexpr auto OgrePath = "ogre.nca";
constexpr auto RampPath = "ramp.nca";
constexpr auto SkeletonPath = "skeleton.nca";

extern asset::MeshView Capsule;
extern asset::MeshView Cave;
extern asset::MeshView Cube;
extern asset::MeshView Guy;
extern asset::MeshView HalfPipe;
extern asset::MeshView Ogre;
extern asset::MeshView Plane;
extern asset::MeshView Ramp;
extern asset::MeshView Skeleton;
extern asset::MeshView Sphere;
} // namespace mesh
} // namespace nc::sample
