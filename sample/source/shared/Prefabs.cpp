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
graphics::ToonMaterial DefaultToonMaterial{asset::DefaultBaseColor, toonOverlay, toonHatch, 1};
graphics::ToonMaterial RedToonMaterial{"solid_color\\Red.nca", toonOverlay, toonHatch, 1};
graphics::ToonMaterial GreenToonMaterial{"solid_color\\Green.nca", toonOverlay, toonHatch, 8};
graphics::ToonMaterial BlueToonMaterial{"solid_color\\Blue.nca", toonOverlay, toonHatch, 1};
graphics::ToonMaterial OrangeToonMaterial{"solid_color\\Orange.nca", toonOverlay, toonHatch, 1};
graphics::ToonMaterial PurpleToonMaterial{"solid_color\\Purple.nca", toonOverlay, toonHatch, 1};
graphics::ToonMaterial TealToonMaterial{"solid_color\\Teal.nca", toonOverlay, toonHatch, 1};
graphics::ToonMaterial YellowToonMaterial{"solid_color\\Yellow.nca", toonOverlay, toonHatch, 1};

graphics::PbrMaterial DefaultPbrMaterial{asset::DefaultBaseColor, asset::DefaultNormal, asset::DefaultRoughness, asset::DefaultMetallic};
graphics::PbrMaterial RedPbrMaterial{"solid_color\\Red.nca", asset::DefaultNormal, asset::DefaultRoughness, asset::DefaultMetallic};
graphics::PbrMaterial GreenPbrMaterial{"solid_color\\Red.nca", asset::DefaultNormal, asset::DefaultRoughness, asset::DefaultMetallic};
graphics::PbrMaterial BluePbrMaterial{"solid_color\\Red.nca", asset::DefaultNormal, asset::DefaultRoughness, asset::DefaultMetallic};
}



namespace nc::sample::prefab
{
    bool IsInitialized = false;

    namespace material
    {
        graphics::ToonMaterial Box{};
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
            "box\\BaseColor.nca",
            "box\\Roughness.nca",
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
            "box\\Normal.nca",
            "logo\\Normal.nca"
        };
        LoadTextureAssets(normalMaps, false, AssetFlags::TextureTypeNormalMap);

        // material::Box        = graphics::PbrMaterial{ "box\\BaseColor.nca",     "box\\Normal.nca",    "box\\Roughness.nca",    asset::DefaultRoughness };
        // material::SolidBlue  = graphics::PbrMaterial{ "solid_color\\Blue.nca",  asset::DefaultNormal, asset::DefaultRoughness, asset::DefaultRoughness };
        // material::SolidGreen = graphics::PbrMaterial{ "solid_color\\Green.nca", asset::DefaultNormal, asset::DefaultRoughness, asset::DefaultRoughness };
        // material::SolidRed   = graphics::PbrMaterial{ "solid_color\\Red.nca",   asset::DefaultNormal, asset::DefaultRoughness, asset::DefaultRoughness };
        // material::Ground     = graphics::PbrMaterial{ "floor\\BaseColor.nca",   "floor\\Normal.nca",  "floor\\Roughness.nca",  asset::DefaultRoughness };
        // material::Worm       = graphics::PbrMaterial{ "logo\\BaseColor.nca",    "logo\\Normal.nca",   "logo\\Roughness.nca",   asset::DefaultRoughness };


    }
} // end namespace project::prefab
