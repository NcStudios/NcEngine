#pragma once

#include "ncengine/asset/AssetViews.h"
#include "ncengine/asset/NcAsset.h"
#include "ncengine/graphics/Material.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/ToonRenderer.h"

namespace nc::sample
{
// Load Asset Files
void InitializeResources();

// 
void ReloadPrefabs();


// Mesh
constexpr auto HalfPipeMesh = "halfpipe.nca";
constexpr auto RampMesh = "ramp.nca";

namespace mesh
{
constexpr auto HalfPipePath = "halfpipe.nca";
constexpr auto RampPath = "ramp.nca";
constexpr auto OgrePath = "ogre.nca";
constexpr auto SkeletonPath = "skeleton.nca";
constexpr auto CavePath = "cave.nca";
constexpr auto CaveCeilingPath = "cave_ceiling.nca";

extern asset::MeshView Cube;
extern asset::MeshView Sphere;
extern asset::MeshView Capsule;
extern asset::MeshView Plane;
extern asset::MeshView HalfPipe;
extern asset::MeshView Ramp;
extern asset::MeshView Ogre;
extern asset::MeshView Skeleton;
extern asset::MeshView Cave;
extern asset::MeshView CaveCeiling;
} // namespace mesh

// Colliders
constexpr auto RampHullCollider = "ramp.nca";
constexpr auto HalfPipeConcaveCollider = "halfpipe.nca";

// Materials
namespace material
{
extern MaterialDesc Default;
extern MaterialDesc Red;
extern MaterialDesc Green;
extern MaterialDesc Blue;
extern MaterialDesc Orange;
extern MaterialDesc Purple;
extern MaterialDesc Teal;
extern MaterialDesc Yellow;
extern MaterialDesc Ogre;
extern MaterialDesc Skeleton;
extern MaterialDesc Cave;
extern MaterialDesc CaveCeiling;
} // namespace material

// Animations
namespace animation
{
extern uint64_t OgreIdle;
extern uint64_t OgreAttack;
extern uint64_t SkeletonIdle;
extern uint64_t SkeletonJump;
extern uint64_t SkeletonWalkRight;
extern uint64_t SkeletonWalkLeft;
extern uint64_t SkeletonWalkForward;
extern uint64_t SkeletonWalkBackward;
} // namespace animation

// CubeMaps
namespace cubemap
{
constexpr auto NightSkyPath = "night_sky.nca";
} // namespace cubemap

// Fonts
extern asset::FontInfo UIFont;
} // namespace nc::sample
