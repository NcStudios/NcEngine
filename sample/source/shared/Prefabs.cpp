#include "Prefabs.h"

#include "ncengine/asset/Assets.h"
#include "ncengine/config/Config.h"

#include <filesystem>

namespace nc::sample
{
bool IsInitialized = false;

graphics::PbrMaterial DefaultPbrMaterial{asset::DefaultBaseColor, asset::DefaultNormal, asset::DefaultRoughness, asset::DefaultMetallic};
graphics::PbrMaterial RedPbrMaterial{"solid_color\\Red.nca", asset::DefaultNormal, asset::DefaultRoughness, asset::DefaultMetallic};
graphics::PbrMaterial GreenPbrMaterial{"solid_color\\Green.nca", asset::DefaultNormal, asset::DefaultRoughness, asset::DefaultMetallic};
graphics::PbrMaterial BluePbrMaterial{"solid_color\\Blue.nca", asset::DefaultNormal, asset::DefaultRoughness, asset::DefaultMetallic};
graphics::PbrMaterial OrangePbrMaterial{"solid_color\\Orange.nca", asset::DefaultNormal, asset::DefaultRoughness, asset::DefaultMetallic};
graphics::PbrMaterial PurplePbrMaterial{"solid_color\\Purple.nca", asset::DefaultNormal, asset::DefaultRoughness, asset::DefaultMetallic};
graphics::PbrMaterial TealPbrMaterial{"solid_color\\Teal.nca", asset::DefaultNormal, asset::DefaultRoughness, asset::DefaultMetallic};
graphics::PbrMaterial YellowPbrMaterial{"solid_color\\Yello.nca", asset::DefaultNormal, asset::DefaultRoughness, asset::DefaultMetallic};

constexpr auto toonOverlay = "line\\overlay.nca";
constexpr auto toonHatch = asset::DefaultBaseColor;
graphics::ToonMaterial DefaultToonMaterial{asset::DefaultBaseColor, toonOverlay, toonHatch, 8};
graphics::ToonMaterial RedToonMaterial{"solid_color\\Red.nca", toonOverlay, toonHatch, 8};
graphics::ToonMaterial GreenToonMaterial{"solid_color\\Green.nca", toonOverlay, toonHatch, 8};
graphics::ToonMaterial BlueToonMaterial{"solid_color\\Blue.nca", toonOverlay, toonHatch, 8};
graphics::ToonMaterial OrangeToonMaterial{"solid_color\\Orange.nca", toonOverlay, toonHatch, 8};
graphics::ToonMaterial PurpleToonMaterial{"solid_color\\Purple.nca", toonOverlay, toonHatch, 8};
graphics::ToonMaterial TealToonMaterial{"solid_color\\Teal.nca", toonOverlay, toonHatch, 8};
graphics::ToonMaterial YellowToonMaterial{"solid_color\\Yellow.nca", toonOverlay, toonHatch, 8};

FontInfo UIFont{"SourceCodePro-Regular.ttf", 14.0f};

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
    LoadFont(UIFont);

    std::vector<std::string> textures
    {
        "box\\BaseColor.nca",
        "box\\Roughness.nca",
        "line\\Hatch3.nca",
        "line\\hatch.nca",
        "line\\overlay.nca",
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
}
} // namespace sample
