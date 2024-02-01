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
void Inspector::Draw(ecs::Ecs world, Entity entity, CreateEntityWindow& createEntityWindow)
{
    ChildWindow("Inspector", [&]()
    {
        if (ImGui::BeginPopupContextWindow(nullptr, g_contextMenuFlags))
        {
            EntityContextMenu(entity, world, createEntityWindow);
            ImGui::EndPopup();
        }

        ElementHeader("Entity");
        DragAndDropSource<Entity>(&entity);
        ImGui::Text("Index %d", entity.Index());
        ImGui::Text("Layer %d", entity.Layer());
        if (ImGui::TreeNodeEx("flags"))
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

        std::ranges::for_each(world.GetComponentPools(), [entity](auto&& pool)
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
