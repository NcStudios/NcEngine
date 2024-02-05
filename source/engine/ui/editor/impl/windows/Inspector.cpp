#include "Inspector.h"
#include "EntityContextMenu.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/ui/ImGuiUtility.h"

#include <string_view>
#include <ranges>

namespace
{
constexpr auto g_contextMenuFlags = ImGuiPopupFlags_NoOpenOverItems |
                                    ImGuiPopupFlags_MouseButtonRight;

void ElementHeader(std::string_view name)
{
    IMGUI_SCOPE(nc::ui::ImGuiId, name.data());
    ImGui::Spacing();
    ImGui::Button(name.data(), {-1, 0});
    ImGui::Spacing();
};
} // anonymous namesapce

namespace nc::ui::editor
{
void Inspector::Draw(EditorContext& ctx)
{
    ChildWindow("Inspector", [&]()
    {
        if (ImGui::BeginPopupContextWindow(nullptr, g_contextMenuFlags))
        {
            EntityContextMenu(ctx.selectedEntity, ctx.world);
            ImGui::EndPopup();
        }

        ElementHeader("Entity");
        DragAndDropSource<Entity>(&ctx.selectedEntity);
        ImGui::Text("Index   %d", ctx.selectedEntity.Index());
        ImGui::Text("Layer   %d", ctx.selectedEntity.Layer());
        ImGui::Text("Static  %s", ctx.selectedEntity.IsStatic() ? "True" : "False");
        std::ranges::for_each(ctx.world.GetComponentPools(), [entity = ctx.selectedEntity](auto&& pool)
        {
            if (pool->HasDrawUI() && pool->Contains(entity))
            {
                auto any = pool->GetAsAnyComponent(entity);
                ElementHeader(any.Name());
                any.DrawUI();
            }
        });
    });
}
} // namespace nc::ui::editor
