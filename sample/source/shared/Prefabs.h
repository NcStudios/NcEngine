#pragma once

#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/ToonRenderer.h"

namespace nc::sample
{
void InitializeResources();

extern graphics::ToonMaterial DefaultToonMaterial;
extern graphics::ToonMaterial RedToonMaterial;
extern graphics::ToonMaterial GreenToonMaterial;
extern graphics::ToonMaterial BlueToonMaterial;
extern graphics::ToonMaterial OrangeToonMaterial;
extern graphics::ToonMaterial PurpleToonMaterial;
extern graphics::ToonMaterial TealToonMaterial;
extern graphics::ToonMaterial YellowToonMaterial;

extern graphics::PbrMaterial DefaultPbrMaterial;
extern graphics::PbrMaterial RedPbrMaterial;
extern graphics::PbrMaterial GreenPbrMaterial;
extern graphics::PbrMaterial BluePbrMaterial;
} // namespace nc::sample
