#include "PostProcessDialog.h"
#include "ncengine/graphics/GraphicsUtility.h"
#include "ncengine/graphics/NcGraphics.h"

#include <concepts>

namespace
{
void DrawPostProcessPassInfo(nc::PostProcessEffectId effectId,
                             nc::PostProcessPassFlag::type passId,
                             nc::NcGraphics* ncGraphics)
{
    ImGui::Separator();
    ImGui::Text("\t%s Pass", nc::GetPostProcessPassName(passId).data());
    if (nc::PassHasProperties(passId))
    {
        std::visit(
            [effectId, ncGraphics](const auto& unpacked){
                using T = std::decay_t<decltype(unpacked)>;
                if constexpr (std::same_as<T, nc::OutlinePassProperties>)
                {
                    IMGUI_SCOPE(nc::ui::Indent);
                    IMGUI_SCOPE(nc::ui::Indent);
                    auto copy = unpacked;
                    auto modified = nc::ui::InputColor3(copy.color, "color");
                    modified = nc::ui::DragFloat(copy.width, "width", 0.005f, 0.0f, 5.0f) || modified;
                    modified = nc::ui::DragFloat(copy.depthThreshold, "depthThreshold", 0.01f, 0.0f, 2.0f) || modified;
                    modified = nc::ui::DragFloat(copy.normalThreshold, "normalThreshold", 0.01f, 0.0f, 2.0f) || modified;
                    if (modified)
                    {
                        ncGraphics->SetPostProcessEffectProperties(effectId, nc::PostProcessPassFlag::Outline, copy);
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
                DrawPostProcessPassInfo(effectId, passId, m_ncGraphics);
            }

            ImGui::Separator();
        }
    });
}
} // namespace nc::ui::editor
