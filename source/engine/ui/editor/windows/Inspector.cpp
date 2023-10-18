#include "Inspector.h"
#include "ncengine/ecs/Registry.h"

#include "imgui/imgui.h"

#include <string_view>

namespace
{
void ElementHeader(std::string_view name)
{
    ImGui::Spacing();
    ImGui::Button(name.data(), {-1, 0});
    ImGui::Spacing();
};
} // anonymous namesapce

namespace nc::ui::editor
{
void Inspector::Draw(Registry* registry, Entity entity)
{
    if (ImGui::BeginChild("Inspector", {0, 0}, true))
    {
        ElementHeader("Entity");
        ImGui::Text("Index   %d", entity.Index());
        ImGui::Text("Layer   %d", entity.Layer());
        ImGui::Text("Static  %s", entity.IsStatic() ? "True" : "False");

        for (auto& any : registry->GetAllComponentsOn(entity))
        {
            if (any.HasDrawUI())
            {
                ElementHeader(any.Name());
                any.DrawUI();
            }
        }

        ImGui::EndChild();
    }
}
} // namespace nc::ui::editor
