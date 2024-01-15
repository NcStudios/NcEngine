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
void Inspector::Draw(ecs::Ecs world, Entity entity)
{
    ChildWindow("Inspector", [&]()
    {
        if (ImGui::BeginPopupContextWindow(nullptr, g_contextMenuFlags))
        {
            EntityContextMenu(entity, world);
            ImGui::EndPopup();
        }

        ElementHeader("Entity");
        DragAndDropSource<Entity>(&entity);
        ImGui::Text("Index        %d", entity.Index());
        ImGui::Text("Layer        %d", entity.Layer());
        ImGui::Text("Static       %s", entity.IsStatic() ? "True" : "False");
        ImGui::Text("Persistent   %s", entity.IsPersistent() ? "True" : "False");
        ImGui::Text("Serializable %s", entity.IsSerializable() ? "True" : "False");

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
