#include "PostProcessDialog.h"
#include "ui/editor/impl/assets/AssetWrapper.h"
#include "ncengine/asset/NcAsset.h"
#include "ncengine/asset/DefaultAssets.h"
#include "ncengine/graphics/GraphicsUtility.h"
#include "ncengine/graphics/NcGraphics.h"

#include "asset/AssetService.h"

#include <concepts>

namespace
{
void DrawPostProcessPassInfo(nc::PostProcessEffectId effectId,
                             nc::PostProcessPassFlag::type passId,
                             nc::NcGraphics* ncGraphics,
                             nc::asset::NcAsset* ncAsset)
{
    ImGui::Separator();
    ImGui::Text("\t%s Pass", nc::GetPostProcessPassName(passId).data());
    if (nc::PassHasProperties(passId))
    {
        std::visit(
            [effectId, ncGraphics, ncAsset](const auto& unpacked){
                using T = std::decay_t<decltype(unpacked)>;
                if constexpr (std::same_as<T, nc::OutlinePassProperties>)
                {
                    IMGUI_SCOPE(nc::ui::Indent);
                    IMGUI_SCOPE(nc::ui::Indent);
                    auto copy = unpacked;
                    auto modified = nc::ui::InputColor3(copy.color, "color");
                    modified = nc::ui::DragFloat(copy.width, "width", 1.0f, 0.0f, 5.0f) || modified;
                    modified = nc::ui::DragFloat(copy.depthThreshold, "depthThreshold", 0.01f, 0.0f, 10.0f) || modified;
                    modified = nc::ui::DragFloat(copy.viewDirDepthThreshold, "viewDirdepthThreshold", 0.01f, 0.0f, 10.0f) || modified;
                    modified = nc::ui::DragFloat(copy.normalThreshold, "normalThreshold", 0.01f, 0.0f, 10.0f) || modified;
                    if (modified)
                    {
                        ncGraphics->SetPostProcessEffectProperties(effectId, nc::PostProcessPassFlag::Outline, copy);
                    }
                }
                else if constexpr (std::same_as<T, nc::NoisePassProperties>)
                {
                    IMGUI_SCOPE(nc::ui::Indent);
                    IMGUI_SCOPE(nc::ui::Indent);
                    auto copy = unpacked;
                    auto modified = nc::ui::InputColor3(copy.maskGradientStart, "noiseMaskGradientStart");
                    modified = nc::ui::DragFloat(copy.maskGradientAmount, "noiseMaskGradientAmount", 0.001f, 0.0f, 1.0f) || modified;
                    modified = nc::ui::InputColor3(copy.maskGradientEnd, "noiseMaskGradientEnd") || modified;

                    static constexpr auto assetType = nc::asset::AssetType::Texture;
                    const auto textureAssets = nc::ui::editor::GetLoadedAssets(assetType);

                    auto noiseTexPath = [&copy, &ncAsset, &modified](){
                        if (copy.noiseTex.id != nc::asset::NullAssetId)
                        {
                            return std::string{ncAsset->GetAssetPath(assetType, copy.noiseTex.id)};
                        }

                        // if no texture is set, just force something
                        modified = true;
                        return std::string{nc::asset::DefaultBaseColor};
                    }();

                    if (nc::ui::Combobox(noiseTexPath, "noise", textureAssets))
                    {
                        modified = true;
                        copy.noiseTex = nc::asset::AssetService<nc::asset::TextureView>::Get()->Acquire(noiseTexPath);
                    }

                    modified = nc::ui::DragFloat(copy.noiseTexAmount, "noiseTexAmount", 0.001f, 0.0f, 1.0f) || modified;
                    modified = nc::ui::DragFloat(copy.noiseTexTiling, "noiseTexTiling", 0.01f, 1.0f, 10.0f) || modified;
                    if (modified)
                    {
                        ncGraphics->SetPostProcessEffectProperties(effectId, nc::PostProcessPassFlag::Noise, copy);
                    }
                }
            },
            ncGraphics->GetPostProcessEffectProperties(effectId, passId)
        );
    }
}
} // anonymous namespace

namespace nc::ui::editor
{
void PostProcessDialog::Draw(const ImVec2& dimensions)
{
    DrawPopup("Post Process FX", dimensions, [&]()
    {
        for (const auto [effectId, effectName] : std::views::zip(GetPostProcessEffectIds(), GetPostProcessEffectNames()))
        {
            ImGui::Separator();
            auto enabled = m_ncGraphics->IsPostProcessEffectEnabled(effectId);
            if (ui::Checkbox(enabled, effectName.data()))
            {
                m_ncGraphics->SetPostProcessEffectEnabled(effectId, enabled);
            }

            IMGUI_SCOPE(ui::DisableIf, !enabled);
            for (const auto passId : GetPostProcessEffectPassFlags(effectId))
            {
                DrawPostProcessPassInfo(effectId, passId, m_ncGraphics, m_ncAsset);
            }

            ImGui::Separator();
        }
    });
}
} // namespace nc::ui::editor
