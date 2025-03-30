#include "EnvironmentDialog.h"
#include "ui/editor/impl/assets/AssetWrapper.h"

#include "ncasset/DefaultAssets.h"
#include "ncengine/asset/NcAsset.h"
#include "ncengine/graphics/GraphicsUtility.h"
#include "ncengine/graphics/NcGraphics.h"

#include "asset/AssetService.h"

#include <concepts>

namespace
{
void DrawEnvironmentInfo(nc::NcGraphics* ncGraphics,
                         nc::asset::NcAsset* ncAsset)
{
    ImGui::Separator();
    ImGui::Text("Skybox");
    ImGui::Separator();

    static constexpr auto assetType = nc::asset::AssetType::CubeMap;
    const auto cubeMapAssets = nc::ui::editor::GetLoadedAssets(assetType);
    auto modified = false;

    auto skyboxId = ncGraphics->GetSkybox();
    auto skyboxPath = std::string();

    if (skyboxId != nc::asset::NullAssetId)
    {
        skyboxPath = std::string{ncAsset->GetAssetPath(assetType, skyboxId)};
    }
    else
    {
        skyboxPath = std::string{nc::asset::DefaultSkyboxCubeMap};
    }

    if (nc::ui::Combobox(skyboxPath, "skybox", cubeMapAssets))
    {
        auto newSkybox = nc::asset::AssetService<nc::asset::CubeMapView>::Get()->Acquire(skyboxPath);
        if (newSkybox.id != skyboxId)
        {
            modified = true;
            ncGraphics->SetSkybox(skyboxPath);
        }
    }
    ImGui::Separator();
}
} // anonymous namespace

namespace nc::ui::editor
{
void EnvironmentDialog::Draw(const ImVec2& dimensions)
{
    DrawPopup("Environment Properties", dimensions, [&]()
    {
        DrawEnvironmentInfo(m_ncGraphics, m_ncAsset);
    });
}
} // namespace nc::ui::editor
