#include "EntityContextMenu.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/ui/ImGuiUtility.h"

#include <ranges>

namespace nc::ui::editor
{
auto EntityContextMenu(Entity entity, ecs::Ecs world) -> Entity
{
    if (ImGui::Selectable("Remove Entity"))
    {
        world.Remove<Entity>(entity);
        return Entity::Null();
    }
    else if (ImGui::Selectable("Add Child"))
    {
        return world.Emplace<Entity>({.parent = entity});
    }
    else if (ImGui::Selectable("Make Root"))
    {
        world.Get<Transform>(entity)->SetParent(Entity::Null());
        return entity;
    }

    if (ImGui::BeginMenu("Modify Components"))
    {
        ModifyComponentList(entity, world);
        ImGui::EndMenu();
    }

    return entity;
}

void ModifyComponentList(Entity entity, ecs::Ecs world)
{
    std::ranges::for_each(world.GetComponentPools(), [entity](auto&& pool)
    {
        if (!pool->HasFactory())
            return;

        const auto name = pool->GetComponentName().data();
        if (pool->Contains(entity))
        {
            if (ImGui::MenuItem(name, "[-]")) pool->Remove(entity);
        }
        else
        {
            if (ImGui::MenuItem(name, "[+]")) pool->AddDefault(entity);
        }
    });
}
} // namespace nc::ui::editor
