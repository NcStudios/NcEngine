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
void DrawEnvironmentInfo(nc::NcGraphics* ncGraphics, nc::asset::NcAsset* ncAsset)
{
    ImGui::Separator();
    ImGui::Text("Skybox");
    ImGui::Separator();

    static constexpr auto assetType = nc::asset::AssetType::CubeMap;
    auto cubeMapAssets = nc::ui::editor::GetLoadedAssets(assetType);

    static const std::string noneOption = "none";
    cubeMapAssets.insert(cubeMapAssets.begin(), noneOption); 

    auto skyboxId = ncGraphics->GetSkybox();
    auto skyboxPath = std::string();

    if (skyboxId != nc::asset::NullAssetId)
    {
        skyboxPath = std::string{ncAsset->GetAssetPath(assetType, skyboxId)};
    }
    else
    {
        skyboxPath = noneOption;
    }

    if (nc::ui::Combobox(skyboxPath, "skybox", cubeMapAssets))
    {
        if (skyboxPath == noneOption)
        {
            ncGraphics->ClearEnvironment();
        }
        else
        {
            auto newSkybox = nc::asset::AssetService<nc::asset::CubeMapView>::Get()->Acquire(skyboxPath);
            if (newSkybox.id != skyboxId)
            {
                ncGraphics->SetSkybox(skyboxPath);
            }
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
