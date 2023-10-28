#include "EntityContextMenu.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/ui/ImGuiUtility.h"

#include <ranges>

namespace nc::ui::editor
{
auto EntityContextMenu(Entity entity, Registry* registry) -> Entity
{
    if (ImGui::Selectable("Remove Entity"))
    {
        registry->Remove<Entity>(entity);
        return Entity::Null();
    }
    else if (ImGui::Selectable("Add Child"))
    {
        return registry->Add<Entity>({.parent = entity});
    }
    else if (ImGui::Selectable("Make Root"))
    {
        registry->Get<Transform>(entity)->SetParent(Entity::Null());
        return entity;
    }

    if (ImGui::BeginMenu("Modify Components"))
    {
        ModifyComponentList(entity, registry);
        ImGui::EndMenu();
    }

    return entity;
}

void ModifyComponentList(Entity entity, Registry* registry)
{
    auto hasFactory = [](auto& pool) { return pool->HasFactory(); };
    for (auto& pool : registry->GetComponentPools() |
                      std::ranges::views::filter(hasFactory))
    {
        const auto name = pool->GetComponentName().data();
        const auto contains = pool->Contains(entity);
        const auto symbol = contains ? "[-]" : "[+]";
        if (ImGui::MenuItem(name, symbol))
        {
            if (contains)
            {
                pool->TryRemove(entity);
            }
            else
            {
                pool->AddDefault(entity);
            }
        }
    }
}
} // namespace nc::ui::editor
