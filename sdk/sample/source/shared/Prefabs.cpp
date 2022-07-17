#include "Prefabs.h"
#include "asset/Assets.h"
#include "ecs/Registry.h"
#include "ecs/component/Collider.h"
#include "ecs/component/ConcaveCollider.h"
#include "ecs/component/MeshRenderer.h"
#include "ecs/component/PhysicsBody.h"
#include "ecs/component/PointLight.h"
#include "ecs/component/Transform.h"

namespace nc::sample::prefab
{
    bool IsInitialized = false;

    std::string ToString(Resource resource)
    {
        switch (resource)
        {
        case Resource::Capsule:
            return std::string{ "Capsule" };
        case Resource::CapsuleBlue:
            return std::string{ "CapsuleBlue" };
        case Resource::CapsuleGreen:
            return std::string{ "CapsuleGreen" };
        case Resource::CapsuleRed:
            return std::string{ "CapsuleRed" };
        case Resource::CubeTextured:
            return std::string{ "CubeTextured" };
        case Resource::Coin:
            return std::string{ "Coin" };
        case Resource::Cube:
            return std::string{ "Cube" };
        case Resource::CubeBlue:
            return std::string{ "CubeBlue" };
        case Resource::CubeGreen:
            return std::string{ "CubeGreen" };
        case Resource::CubeRed:
            return std::string{ "CubeRed" };
        case Resource::Disc:
            return std::string{ "Disc" };
        case Resource::DiscBlue:
            return std::string{ "DiscBlue" };
        case Resource::DiscGreen:
            return std::string{ "DiscGreen" };
        case Resource::DiscRed:
            return std::string{ "DiscRed" };
        case Resource::Ground:
            return std::string{ "Ground" };
        case Resource::RampRed:
            return std::string{ "RampRed" };
        case Resource::Sphere:
            return std::string{ "Sphere" };
        case Resource::SphereBlue:
            return std::string{ "SphereBlue" };
        case Resource::SphereGreen:
            return std::string{ "SphereGreen" };
        case Resource::SphereRed:
            return std::string{ "SphereRed" };
        case Resource::PlaneGreen:
            return std::string{ "PlaneGreen" };
        case Resource::Table:
            return std::string{ "Table" };
        case Resource::Token:
            return std::string{ "Token" };
        case Resource::Worm:
            return std::string{ "Worm" };
        }
        throw NcError("prefab::ToString(resource) - Unknown resource");
    }

    namespace material
    {
        Material Box{};
        Material Coin{};
        Material Ground{};
        Material SolidBlue{};
        Material SolidGreen{};
        Material SolidRed{};
        Material Default{};
        Material Table{};
        Material Token{};
        Material Worm{};
    } // end namespace material

    void InitializeResources()
    {
        if (IsInitialized)
        {
            return;
        }

        IsInitialized = true;

        LoadAudioClipAsset("hit.wav");
        LoadAudioClipAsset("drums.wav");

        LoadConvexHullAsset("coin.nca");
        LoadConcaveColliderAsset("plane.nca");
        LoadConcaveColliderAsset("ramp.nca");

        auto meshPaths = std::vector<std::string>{ "capsule.nca",
                                                   "coin.nca",
                                                   "cube.nca",
                                                   "plane.nca",
                                                   "planet.nca",
                                                   "ramp.nca",
                                                   "sphere.nca",
                                                   "table.nca",
                                                   "token.nca",
                                                   "worm.nca" };
        nc::LoadMeshAssets(meshPaths);

        const auto defaultBaseColor = std::string{ "DefaultBaseColor.png" };
        const auto defaultNormal = std::string{ "DefaultNormal.png" };
        const auto defaultRoughness = std::string{ "DefaultMetallic.png" };

        const std::vector<std::string> texturePaths{ defaultBaseColor,
                                                     defaultNormal,
                                                     defaultRoughness,
                                                     "beeper/BaseColor.png",
                                                     "beeper/Normal.png",
                                                     "box/BaseColor.png",
                                                     "box/Normal.png",
                                                     "box/Roughness.png",
                                                     "beeper/Roughness.png",
                                                     "floor/BaseColor.png",
                                                     "floor/Normal.png",
                                                     "floor/Roughness.png",
                                                     "solid_color/Blue.png",
                                                     "solid_color/Green.png",
                                                     "solid_color/Red.png",
                                                     "coin/BaseColor.png",
                                                     "coin/Normal.png",
                                                     "coin/Roughness.png",
                                                     "table/BaseColor.png",
                                                     "table/Normal.png",
                                                     "table/Roughness.png",
                                                     "token/BaseColor.png",
                                                     "token/Normal.png",
                                                     "token/Roughness.png",
                                                     "logo/BaseColor.png",
                                                     "logo/Normal.png",
                                                     "logo/Roughness.png" };
        nc::LoadTextureAssets(texturePaths);

        // @todo can remove once relative paths are working
        //nc::LoadMeshAssets(std::vector<std::string>{"nc/resources/mesh/capsule.nca", "nc/resources/mesh/cube.nca", "nc/resources/mesh/plane.nca", "nc/resources/mesh/sphere.nca"});

        material::Box        = Material{ "box/BaseColor.png",     "box/Normal.png",   "box/Roughness.png",   defaultRoughness };
        material::SolidBlue  = Material{ "solid_color/Blue.png",  defaultNormal,      defaultRoughness,      defaultRoughness };
        material::SolidGreen = Material{ "solid_color/Green.png", defaultNormal,      defaultRoughness,      defaultRoughness };
        material::SolidRed   = Material{ "solid_color/Red.png",   defaultNormal,      defaultRoughness,      defaultRoughness };
        material::Coin       = Material{ "coin/BaseColor.png",    "coin/Normal.png",  "coin/Roughness.png",  defaultRoughness };
        material::Default    = Material{ defaultBaseColor,        defaultNormal,      defaultRoughness,      defaultRoughness };
        material::Ground     = Material{ "floor/BaseColor.png",   "floor/Normal.png", "floor/Roughness.png", defaultRoughness };
        material::Table      = Material{ "table/BaseColor.png",   "table/Normal.png", "table/Roughness.png", defaultRoughness };
        material::Token      = Material{ "token/BaseColor.png",   "token/Normal.png", "token/Roughness.png", defaultRoughness };
        material::Worm       = Material{ "logo/BaseColor.png",    "logo/Normal.png",  "logo/Roughness.png",  defaultRoughness };
    }

    template<Resource Resource_t>
    Entity Create_(Registry*, EntityInfo);

    template<> Entity Create_<Resource::Capsule>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<MeshRenderer>(handle, "capsule.nca", material::Default, TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::CapsuleBlue>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<MeshRenderer>(handle, "capsule.nca", material::SolidBlue, TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::CapsuleGreen>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<MeshRenderer>(handle, "capsule.nca", material::SolidGreen, TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::CapsuleRed>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<MeshRenderer>(handle, "capsule.nca", material::SolidRed, TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::Coin>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<MeshRenderer>(handle, "coin.nca", material::Coin, TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::Cube>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<MeshRenderer>(handle, "cube.nca", material::Default, TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::CubeBlue>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<MeshRenderer>(handle, "cube.nca", material::SolidBlue, TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::CubeGreen>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<MeshRenderer>(handle, "cube.nca", material::SolidGreen, TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::CubeRed>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<MeshRenderer>(handle, "cube.nca", material::SolidRed, TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::CubeTextured>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<MeshRenderer>(handle, "cube.nca", material::Box, TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::Disc>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<MeshRenderer>(handle, "coin.nca", material::Default, TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::DiscBlue>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<MeshRenderer>(handle, "coin.nca", material::SolidBlue, TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::DiscGreen>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<MeshRenderer>(handle, "coin.nca", material::SolidGreen, TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::DiscRed>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<MeshRenderer>(handle, "coin.nca", material::SolidRed, TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::Ground>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<MeshRenderer>(handle, "cube.nca", material::Ground, TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::RampRed>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<MeshRenderer>(handle, "ramp.nca", material::SolidRed, TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::Sphere>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<MeshRenderer>(handle, "sphere.nca", material::Default, TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::SphereBlue>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<MeshRenderer>(handle, "sphere.nca", material::SolidBlue, TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::SphereGreen>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<MeshRenderer>(handle, "sphere.nca", material::SolidGreen, TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::SphereRed>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<MeshRenderer>(handle, "sphere.nca", material::SolidRed, TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::PlaneGreen>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<MeshRenderer>(handle, "plane.nca", material::SolidGreen, TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::Table>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<MeshRenderer>(handle, "table.nca", material::Table, TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::Token>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<MeshRenderer>(handle, "token.nca", material::Token, TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::Worm>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<MeshRenderer>(handle, "worm.nca", material::Worm, TechniqueType::PhongAndUi);
        return handle;
    }

    using CreateFunc_t = Entity(*)(Registry* registry, EntityInfo info);

    const auto dispatch = std::unordered_map<prefab::Resource, CreateFunc_t>
    {
        std::pair{Resource::Capsule,       Create_<Resource::Capsule>},
        std::pair{Resource::CapsuleBlue,   Create_<Resource::CapsuleBlue>},
        std::pair{Resource::CapsuleGreen,  Create_<Resource::CapsuleGreen>},
        std::pair{Resource::CapsuleRed,    Create_<Resource::CapsuleRed>},
        std::pair{Resource::Coin,          Create_<Resource::Coin>},
        std::pair{Resource::Cube,          Create_<Resource::Cube>},
        std::pair{Resource::CubeBlue,      Create_<Resource::CubeBlue>},
        std::pair{Resource::CubeGreen,     Create_<Resource::CubeGreen>},
        std::pair{Resource::CubeRed,       Create_<Resource::CubeRed>},
        std::pair{Resource::CubeTextured,  Create_<Resource::CubeTextured>},
        std::pair{Resource::Disc,          Create_<Resource::Disc>},
        std::pair{Resource::DiscBlue,      Create_<Resource::DiscBlue>},
        std::pair{Resource::DiscGreen,     Create_<Resource::DiscGreen>},
        std::pair{Resource::DiscRed,       Create_<Resource::DiscRed>},
        std::pair{Resource::Ground,        Create_<Resource::Ground>},
        std::pair{Resource::RampRed,       Create_<Resource::RampRed>},
        std::pair{Resource::Sphere,        Create_<Resource::Sphere>},
        std::pair{Resource::SphereBlue,    Create_<Resource::SphereBlue>},
        std::pair{Resource::SphereGreen,   Create_<Resource::SphereGreen>},
        std::pair{Resource::SphereRed,     Create_<Resource::SphereRed>},
        std::pair{Resource::PlaneGreen,    Create_<Resource::PlaneGreen>},
        std::pair{Resource::Table,         Create_<Resource::Table>},
        std::pair{Resource::Token,         Create_<Resource::Token>},
        std::pair{Resource::Worm,          Create_<Resource::Worm>}
    };

    nc::Entity Create(Registry* registry, Resource resource, EntityInfo info)
    {
        return dispatch.at(resource)(registry, std::move(info));
    }
} // end namespace project::prefab