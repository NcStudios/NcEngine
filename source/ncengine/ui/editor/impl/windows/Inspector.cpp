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
void Inspector::Draw(EditorContext& ctx, CreateEntityDialog& createEntity)
{
    ChildWindow("Inspector", [&]()
    {
        auto entity = ctx.selectedEntity;
        if (ImGui::BeginPopupContextWindow(nullptr, g_contextMenuFlags))
        {
            EntityContextMenu(ctx.selectedEntity, ctx.world, createEntity);
            ImGui::EndPopup();
        }

        ElementHeader("Entity");
        DragAndDropSource<Entity>(&entity);
        ImGui::Text("Index: %d", entity.Index());
        ImGui::Text("Layer: %d", entity.Layer());
        if (ImGui::TreeNodeEx("Flags"))
        {
            ImGui::BeginDisabled(true);
            auto isStatic = entity.IsStatic();
            auto isPersistent = entity.IsPersistent();
            auto collisionEvents = entity.ReceivesCollisionEvents();
            auto serializable = entity.IsSerializable();
            auto isInternal = entity.IsInternal();
            Checkbox(isStatic, "static");
            Checkbox(isPersistent, "persistent");
            Checkbox(collisionEvents, "collisionEvents");
            Checkbox(serializable, "serializable");
            Checkbox(isInternal, "internal");
            ImGui::EndDisabled();
            ImGui::TreePop();
        }

        std::ranges::for_each(ctx.world.GetComponentPools(), [entity, &ctx](auto&& pool)
        {
            if (pool->HasDrawUI() && pool->Contains(entity))
            {
                auto any = pool->GetAsAnyComponent(entity);
                ElementHeader(any.Name());
                any.DrawUI(ctx);
            }
        });
    });
}
} // namespace nc::ui::editor
