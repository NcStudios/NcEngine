#include "Prefabs.h"

#include "ncengine/asset/Assets.h"
#include "ncengine/config/Config.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/PointLight.h"
#include "ncengine/physics/Collider.h"
#include "ncengine/physics/ConcaveCollider.h"
#include "ncengine/physics/PhysicsBody.h"

#include <filesystem>

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
        graphics::PbrMaterial Box{};
        graphics::PbrMaterial Coin{};
        graphics::PbrMaterial Ground{};
        graphics::PbrMaterial SolidBlue{};
        graphics::PbrMaterial SolidGreen{};
        graphics::PbrMaterial SolidRed{};
        graphics::PbrMaterial Default{};
        graphics::PbrMaterial Table{};
        graphics::PbrMaterial Token{};
        graphics::PbrMaterial Worm{};
    } // end namespace material

    template<class LoadFunc>
    void LoadAssets(const std::filesystem::path& rootDir, LoadFunc load)
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

        load(paths, false);
    }

    void InitializeResources()
    {
        if (IsInitialized)
        {
            return;
        }

        IsInitialized = true;

        const auto& assetSettings = config::GetAssetSettings();
        LoadAssets(assetSettings.audioClipsPath, &LoadAudioClipAssets);
        LoadAssets(assetSettings.concaveCollidersPath, &LoadConcaveColliderAssets);
        LoadAssets(assetSettings.cubeMapsPath, &LoadCubeMapAssets);
        LoadAssets(assetSettings.hullCollidersPath, &LoadConvexHullAssets);
        LoadAssets(assetSettings.meshesPath, &LoadMeshAssets);
        LoadAssets(assetSettings.texturesPath, &LoadTextureAssets);

        const auto defaultBaseColor = std::string{ "DefaultBaseColor.nca" };
        const auto defaultNormal = std::string{ "DefaultNormal.nca" };
        const auto defaultRoughness = std::string{ "DefaultMetallic.nca" };

        material::Box        = graphics::PbrMaterial{ "box\\BaseColor.nca",     "box\\Normal.nca",   "box\\Roughness.nca",   defaultRoughness };
        material::SolidBlue  = graphics::PbrMaterial{ "solid_color\\Blue.nca",  defaultNormal,      defaultRoughness,      defaultRoughness };
        material::SolidGreen = graphics::PbrMaterial{ "solid_color\\Green.nca", defaultNormal,      defaultRoughness,      defaultRoughness };
        material::SolidRed   = graphics::PbrMaterial{ "solid_color\\Red.nca",   defaultNormal,      defaultRoughness,      defaultRoughness };
        material::Coin       = graphics::PbrMaterial{ "coin\\BaseColor.nca",    "coin\\Normal.nca",  "coin\\Roughness.nca",  defaultRoughness };
        material::Default    = graphics::PbrMaterial{ defaultBaseColor,        defaultNormal,      defaultRoughness,      defaultRoughness };
        material::Ground     = graphics::PbrMaterial{ "floor\\BaseColor.nca",   "floor\\Normal.nca", "floor\\Roughness.nca", defaultRoughness };
        material::Table      = graphics::PbrMaterial{ "table\\BaseColor.nca",   "table\\Normal.nca", "table\\Roughness.nca", defaultRoughness };
        material::Token      = graphics::PbrMaterial{ "token\\BaseColor.nca",   "token\\Normal.nca", "token\\Roughness.nca", defaultRoughness };
        material::Worm       = graphics::PbrMaterial{ "logo\\BaseColor.nca",    "logo\\Normal.nca",  "logo\\Roughness.nca",  defaultRoughness };
    }

    template<Resource Resource_t>
    Entity Create_(Registry*, EntityInfo);

    template<> Entity Create_<Resource::Capsule>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::MeshRenderer>(handle, "capsule.nca", material::Default, graphics::TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::CapsuleBlue>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::MeshRenderer>(handle, "capsule.nca", material::SolidBlue, graphics::TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::CapsuleGreen>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::MeshRenderer>(handle, "capsule.nca", material::SolidGreen, graphics::TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::CapsuleRed>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::MeshRenderer>(handle, "capsule.nca", material::SolidRed, graphics::TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::Coin>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::MeshRenderer>(handle, "coin.nca", material::Coin, graphics::TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::Cube>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::MeshRenderer>(handle, "cube.nca", material::Default, graphics::TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::CubeBlue>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::MeshRenderer>(handle, "cube.nca", material::SolidBlue, graphics::TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::CubeGreen>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::MeshRenderer>(handle, "cube.nca", material::SolidGreen, graphics::TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::CubeRed>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::MeshRenderer>(handle, "cube.nca", material::SolidRed, graphics::TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::CubeTextured>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::MeshRenderer>(handle, "cube.nca", material::Box, graphics::TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::Disc>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::MeshRenderer>(handle, "coin.nca", material::Default, graphics::TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::DiscBlue>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::MeshRenderer>(handle, "coin.nca", material::SolidBlue, graphics::TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::DiscGreen>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::MeshRenderer>(handle, "coin.nca", material::SolidGreen, graphics::TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::DiscRed>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::MeshRenderer>(handle, "coin.nca", material::SolidRed, graphics::TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::Ground>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::MeshRenderer>(handle, "cube.nca", material::Ground, graphics::TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::RampRed>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::MeshRenderer>(handle, "ramp.nca", material::SolidRed, graphics::TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::Sphere>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::MeshRenderer>(handle, "sphere.nca", material::Default, graphics::TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::SphereBlue>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::MeshRenderer>(handle, "sphere.nca", material::SolidBlue, graphics::TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::SphereGreen>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::MeshRenderer>(handle, "sphere.nca", material::SolidGreen, graphics::TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::SphereRed>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::MeshRenderer>(handle, "sphere.nca", material::SolidRed, graphics::TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::PlaneGreen>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::MeshRenderer>(handle, "plane.nca", material::SolidGreen, graphics::TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::Table>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::MeshRenderer>(handle, "table.nca", material::Table, graphics::TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::Token>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::MeshRenderer>(handle, "token.nca", material::Token, graphics::TechniqueType::PhongAndUi);
        return handle;
    }

    template<> Entity Create_<Resource::Worm>(Registry* registry, EntityInfo info)
    {
        auto handle = registry->Add<Entity>(std::move(info));
        registry->Add<graphics::MeshRenderer>(handle, "worm.nca", material::Worm, graphics::TechniqueType::PhongAndUi);
        return handle;
    }

    using CreateFunc_t = Entity(*)(Registry* registry, EntityInfo info);

    const auto dispatch = std::unordered_map<prefab::Resource, CreateFunc_t>
    {
        std::pair{Resource::Capsule,       &Create_<Resource::Capsule>},
        std::pair{Resource::CapsuleBlue,   &Create_<Resource::CapsuleBlue>},
        std::pair{Resource::CapsuleGreen,  &Create_<Resource::CapsuleGreen>},
        std::pair{Resource::CapsuleRed,    &Create_<Resource::CapsuleRed>},
        std::pair{Resource::Coin,          &Create_<Resource::Coin>},
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
        std::pair{Resource::Table,         &Create_<Resource::Table>},
        std::pair{Resource::Token,         &Create_<Resource::Token>},
        std::pair{Resource::Worm,          &Create_<Resource::Worm>}
    };

    nc::Entity Create(Registry* registry, Resource resource, EntityInfo info)
    {
        return dispatch.at(resource)(registry, std::move(info));
    }
} // end namespace project::prefab