#pragma once

#include "ncengine/ecs/Entity.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/ToonRenderer.h"

namespace nc { class Registry; }


namespace nc
{
class Registry;

namespace sample::prefab
{
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


} // namespace sample::asset
} // namespace nc




namespace nc::sample::prefab
{
    void InitializeResources();
} // end namespace project::prefab
