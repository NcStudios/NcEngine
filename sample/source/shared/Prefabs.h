#pragma once

#include "ncengine/asset/AssetViews.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/ToonRenderer.h"

namespace nc::sample
{
void InitializeResources();

// Mesh
constexpr auto HalfPipeMesh = "halfpipe.nca";
constexpr auto RampMesh = "ramp.nca";

// Colliders
constexpr auto RampHullCollider = "ramp.nca";
constexpr auto HalfPipeConcaveCollider = "halfpipe.nca";

// Materials
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

// Fonts
extern FontInfo UIFont;
} // namespace nc::sample
