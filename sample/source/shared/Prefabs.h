#pragma once

#include "ncengine/ecs/Entity.h"
#include "ncengine/graphics/ToonRenderer.h"

namespace nc { class Registry; }


namespace nc
{
class Registry;

namespace sample::prefab
{
extern graphics::ToonMaterial GreenToonMaterial;
extern graphics::ToonMaterial RedToonMaterial;
extern graphics::ToonMaterial BlueToonMaterial;
extern graphics::ToonMaterial OrangeToonMaterial;
extern graphics::ToonMaterial PurpleToonMaterial;
extern graphics::ToonMaterial TealToonMaterial;
extern graphics::ToonMaterial YellowToonMaterial;

} // namespace sample::asset
} // namespace nc




namespace nc::sample::prefab
{
    enum class Resource2 : uint32_t
    {
        Capsule, //
        Cube, //
        Sphere, //
        Plane //
    };

    auto Create(Registry* registry, Resource2 resource, EntityInfo info, const graphics::ToonMaterial& material);


    enum class Resource : uint32_t
    {
        Capsule, //
        CapsuleBlue,
        CapsuleGreen,
        CapsuleRed,
        Cube, //
        CubeBlue,
        CubeGreen,
        CubeRed,
        CubeTextured,
        Disc, //
        DiscBlue,
        DiscGreen,
        DiscRed,
        Ground,
        RampRed,
        Sphere, //
        SphereBlue,
        SphereGreen,
        SphereRed,
        PlaneGreen, //
        Worm
    };

    std::string ToString(Resource resource);
    void InitializeResources();
    Entity Create(Registry* registry, Resource resource, EntityInfo info);
} // end namespace project::prefab
