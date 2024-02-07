#include "Prefabs.h"

#include "ncengine/asset/Assets.h"
#include "ncengine/config/Config.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "ncengine/graphics/PointLight.h"
#include "ncengine/physics/Collider.h"
#include "ncengine/physics/ConcaveCollider.h"
#include "ncengine/physics/PhysicsBody.h"

#include <filesystem>



namespace nc::sample::prefab
{
    constexpr auto toonOverlay = "box\\BaseColor.nca";
    constexpr auto toonHatch = asset::DefaultBaseColor;
    graphics::ToonMaterial GreenToonMaterial{"solid_color\\Green.nca", toonOverlay, toonHatch, 8};
    graphics::ToonMaterial RedToonMaterial{"solid_color\\Red.nca", toonOverlay, toonHatch, 1};
    graphics::ToonMaterial BlueToonMaterial{"solid_color\\Blue.nca", toonOverlay, toonHatch, 1};
    graphics::ToonMaterial OrangeToonMaterial{"solid_color\\Orange.nca", toonOverlay, toonHatch, 1};
    graphics::ToonMaterial PurpleToonMaterial{"solid_color\\Purple.nca", toonOverlay, toonHatch, 1};
    graphics::ToonMaterial TealToonMaterial{"solid_color\\Teal.nca", toonOverlay, toonHatch, 1};
    graphics::ToonMaterial YellowToonMaterial{"solid_color\\Yellow.nca", toonOverlay, toonHatch, 1};

}



namespace nc::sample::prefab
{
// auto Box(Registry* registry, const EntityInfo& info, const graphics::ToonMaterial& material, bool hasCollider = false) -> Entity
// {
//     auto entity = registry->Add<Entity>(info);
//     registry->Add<graphics::ToonRenderer>(entity, asset::CubeMesh, material);
//     if (hasCollider)
//     {

//     }
// }
}


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
        case Resource::Worm:
            return std::string{ "Worm" };
        }
        throw NcError("prefab::ToString(resource) - Unknown resource");
    }

    namespace material
    {
        graphics::ToonMaterial Box{};
        graphics::ToonMaterial Ground{};
        graphics::ToonMaterial SolidBlue{};
        graphics::ToonMaterial SolidGreen{};
        graphics::ToonMaterial SolidRed{};
        graphics::ToonMaterial Default{};
        graphics::ToonMaterial Worm{};
    } // end namespace material

    template<class LoadFunc>
    void LoadAssets(const std::filesystem::path& rootDir, asset_flags_type flags, LoadFunc load)
    {
        auto paths = std::vector<std::string>{};
        for (auto&& entry : std::filesystem::recursive_directory_iterator{rootDir})
        {
            if (entry.path().extension() != ".nca")
            {
                continue;
            }

            const auto& path = entry.path();
            auto trimmedPath = std::filesystem::path{};
            auto segment = path.begin();
            ++segment; ++segment;
            for (; segment != path.end(); ++segment)
            {
                trimmedPath /= *segment;
            }

            paths.push_back(trimmedPath.string());
        }

        load(paths, false, flags);
    }

    void InitializeResources()
    {
        if (IsInitialized)
        {
            return;
        }

        IsInitialized = true;

        const auto& assetSettings = config::GetAssetSettings();
        LoadAssets(assetSettings.audioClipsPath, AssetFlags::None, &LoadAudioClipAssets);
        LoadAssets(assetSettings.concaveCollidersPath, AssetFlags::None, &LoadConcaveColliderAssets);
        LoadAssets(assetSettings.cubeMapsPath, AssetFlags::None, &LoadCubeMapAssets);
        LoadAssets(assetSettings.hullCollidersPath, AssetFlags::None, &LoadConvexHullAssets);
        LoadAssets(assetSettings.meshesPath, AssetFlags::None, &LoadMeshAssets);

        std::vector<std::string> textures
        {
            "blacktop\\BaseColor.nca",
            "blacktop\\Roughness.nca",
            "box\\BaseColor.nca",
            "box\\Roughness.nca",
            "floor\\BaseColor.nca",
            "floor\\Roughness.nca",
            "line\\Hatch3.nca",
            "logo\\BaseColor.nca",
            "logo\\Metallic.nca",
            "logo\\Roughness.nca",
            "solid_color\\Blue.nca",
            "solid_color\\Green.nca",
            "solid_color\\Red.nca",
            "solid_color\\Orange.nca",
            "solid_color\\Purple.nca",
            "solid_color\\Teal.nca",
            "solid_color\\Yellow.nca",
            "tree\\BaseColor.nca"
        };
        LoadTextureAssets(textures, false, AssetFlags::TextureTypeImage);

        std::vector<std::string> normalMaps 
        {
            "blacktop\\Normal.nca",
            "box\\Normal.nca",
            "floor\\Normal.nca",
            "logo\\Normal.nca"
        };
        LoadTextureAssets(normalMaps, false, AssetFlags::TextureTypeNormalMap);

        // material::Box        = graphics::PbrMaterial{ "box\\BaseColor.nca",     "box\\Normal.nca",    "box\\Roughness.nca",    asset::DefaultRoughness };
        // material::SolidBlue  = graphics::PbrMaterial{ "solid_color\\Blue.nca",  asset::DefaultNormal, asset::DefaultRoughness, asset::DefaultRoughness };
        // material::SolidGreen = graphics::PbrMaterial{ "solid_color\\Green.nca", asset::DefaultNormal, asset::DefaultRoughness, asset::DefaultRoughness };
        // material::SolidRed   = graphics::PbrMaterial{ "solid_color\\Red.nca",   asset::DefaultNormal, asset::DefaultRoughness, asset::DefaultRoughness };
        // material::Ground     = graphics::PbrMaterial{ "floor\\BaseColor.nca",   "floor\\Normal.nca",  "floor\\Roughness.nca",  asset::DefaultRoughness };
        // material::Worm       = graphics::PbrMaterial{ "logo\\BaseColor.nca",    "logo\\Normal.nca",   "logo\\Roughness.nca",   asset::DefaultRoughness };

        material::SolidBlue  = graphics::ToonMaterial{ "solid_color\\Teal.nca",  asset::DefaultBaseColor, asset::DefaultBaseColor, 1 };
        material::SolidGreen = graphics::ToonMaterial{ "solid_color\\Orange.nca", asset::DefaultBaseColor, asset::DefaultBaseColor, 1 };
        material::SolidRed   = graphics::ToonMaterial{ "solid_color\\Purple.nca",   asset::DefaultBaseColor, asset::DefaultBaseColor, 1 };


    }




    template<Resource Resource_t>
    Entity Create_(Registry*, EntityInfo);

    template<> Entity Create_<Resource::Capsule>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::ToonRenderer>(handle, asset::CapsuleMesh);
        return handle;
    }






    template<> Entity Create_<Resource::CapsuleBlue>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::ToonRenderer>(handle, asset::CapsuleMesh, material::SolidBlue);
        return handle;
    }

    template<> Entity Create_<Resource::CapsuleGreen>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::ToonRenderer>(handle, asset::CapsuleMesh, material::SolidGreen);
        return handle;
    }

    template<> Entity Create_<Resource::CapsuleRed>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::ToonRenderer>(handle, asset::CapsuleMesh, material::SolidRed);
        return handle;
    }

    template<> Entity Create_<Resource::Cube>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::ToonRenderer>(handle, asset::CubeMesh);
        return handle;
    }

    template<> Entity Create_<Resource::CubeBlue>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::ToonRenderer>(handle, asset::CubeMesh, material::SolidBlue);
        return handle;
    }

    template<> Entity Create_<Resource::CubeGreen>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::ToonRenderer>(handle, asset::CubeMesh, GreenToonMaterial);
        return handle;
    }

    template<> Entity Create_<Resource::CubeRed>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::ToonRenderer>(handle, asset::CubeMesh, material::SolidRed);
        return handle;
    }

    template<> Entity Create_<Resource::CubeTextured>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::ToonRenderer>(handle, asset::CubeMesh, material::Box);
        return handle;
    }

    template<> Entity Create_<Resource::Disc>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::ToonRenderer>(handle, "coin.nca");
        return handle;
    }

    template<> Entity Create_<Resource::DiscBlue>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::ToonRenderer>(handle, "coin.nca", material::SolidBlue);
        return handle;
    }

    template<> Entity Create_<Resource::DiscGreen>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::ToonRenderer>(handle, "coin.nca", material::SolidGreen);
        return handle;
    }

    template<> Entity Create_<Resource::DiscRed>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::ToonRenderer>(handle, "coin.nca", material::SolidRed);
        return handle;
    }

    template<> Entity Create_<Resource::Ground>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::ToonRenderer>(handle, asset::CubeMesh, material::Ground);
        return handle;
    }

    template<> Entity Create_<Resource::RampRed>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::ToonRenderer>(handle, "ramp.nca", RedToonMaterial);
        return handle;
    }

    template<> Entity Create_<Resource::Sphere>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::ToonRenderer>(handle, asset::SphereMesh);
        return handle;
    }

    template<> Entity Create_<Resource::SphereBlue>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::ToonRenderer>(handle, asset::SphereMesh, material::SolidBlue);
        return handle;
    }

    template<> Entity Create_<Resource::SphereGreen>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::ToonRenderer>(handle, asset::SphereMesh, material::SolidGreen);
        return handle;
    }

    template<> Entity Create_<Resource::SphereRed>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::ToonRenderer>(handle, asset::SphereMesh, material::SolidRed);
        return handle;
    }

    template<> Entity Create_<Resource::PlaneGreen>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::ToonRenderer>(handle, asset::PlaneMesh, material::SolidGreen);
        return handle;
    }

    template<> Entity Create_<Resource::Worm>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::ToonRenderer>(handle, "worm.nca", material::Worm);
        return handle;
    }

    using CreateFunc_t = Entity(*)(Registry* registry, EntityInfo info);

    const auto dispatch = std::unordered_map<prefab::Resource, CreateFunc_t>
    {
        std::pair{Resource::Capsule,       &Create_<Resource::Capsule>},
        std::pair{Resource::CapsuleBlue,   &Create_<Resource::CapsuleBlue>},
        std::pair{Resource::CapsuleGreen,  &Create_<Resource::CapsuleGreen>},
        std::pair{Resource::CapsuleRed,    &Create_<Resource::CapsuleRed>},
        std::pair{Resource::Cube,          &Create_<Resource::Cube>},
        std::pair{Resource::CubeBlue,      &Create_<Resource::CubeBlue>},
        std::pair{Resource::CubeGreen,     &Create_<Resource::CubeGreen>},
        std::pair{Resource::CubeRed,       &Create_<Resource::CubeRed>},
        std::pair{Resource::CubeTextured,  &Create_<Resource::CubeTextured>},
        std::pair{Resource::Disc,          &Create_<Resource::Disc>},
        std::pair{Resource::DiscBlue,      &Create_<Resource::DiscBlue>},
        std::pair{Resource::DiscGreen,     &Create_<Resource::DiscGreen>},
        std::pair{Resource::DiscRed,       &Create_<Resource::DiscRed>},
        std::pair{Resource::Ground,        &Create_<Resource::Ground>},
        std::pair{Resource::RampRed,       &Create_<Resource::RampRed>},
        std::pair{Resource::Sphere,        &Create_<Resource::Sphere>},
        std::pair{Resource::SphereBlue,    &Create_<Resource::SphereBlue>},
        std::pair{Resource::SphereGreen,   &Create_<Resource::SphereGreen>},
        std::pair{Resource::SphereRed,     &Create_<Resource::SphereRed>},
        std::pair{Resource::PlaneGreen,    &Create_<Resource::PlaneGreen>},
        std::pair{Resource::Worm,          &Create_<Resource::Worm>}
    };

    nc::Entity Create(Registry* registry, Resource resource, EntityInfo info)
    {
        return dispatch.at(resource)(registry, std::move(info));
    }
} // end namespace project::prefab
