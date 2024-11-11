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

extern asset::MeshView Cube;
extern asset::MeshView Sphere;
extern asset::MeshView Capsule;
extern asset::MeshView Plane;
extern asset::MeshView HalfPipe;
extern asset::MeshView Ramp;
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
} // namespace material

extern graphics::PbrMaterial DefaultPbrMaterial;
extern graphics::PbrMaterial RedPbrMaterial;
extern graphics::PbrMaterial GreenPbrMaterial;
extern graphics::PbrMaterial BluePbrMaterial;
extern graphics::PbrMaterial OrangePbrMaterial;
extern graphics::PbrMaterial PurplePbrMaterial;
extern graphics::PbrMaterial TealPbrMaterial;
extern graphics::PbrMaterial YellowPbrMaterial;

extern graphics::ToonMaterial DefaultToonMaterial;
extern graphics::ToonMaterial RedToonMaterial;
extern graphics::ToonMaterial GreenToonMaterial;
extern graphics::ToonMaterial BlueToonMaterial;
extern graphics::ToonMaterial OrangeToonMaterial;
extern graphics::ToonMaterial PurpleToonMaterial;
extern graphics::ToonMaterial TealToonMaterial;
extern graphics::ToonMaterial YellowToonMaterial;

extern graphics::ToonMaterial DefaultHatchedToonMaterial;
extern graphics::ToonMaterial RedHatchedToonMaterial;
extern graphics::ToonMaterial GreenHatchedToonMaterial;
extern graphics::ToonMaterial BlueHatchedToonMaterial;
extern graphics::ToonMaterial OrangeHatchedToonMaterial;
extern graphics::ToonMaterial PurpleHatchedToonMaterial;
extern graphics::ToonMaterial TealHatchedToonMaterial;
extern graphics::ToonMaterial YellowHatchedToonMaterial;

// Fonts
extern asset::FontInfo UIFont;
} // namespace nc::sample
