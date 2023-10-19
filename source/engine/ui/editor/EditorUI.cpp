#include "EditorUI.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/ui/ImGuiUtility.h"

namespace
{
constexpr auto g_graphInitialGeometry = nc::ui::WindowGeometry
{
    .position = ImVec2{0.0f, 0.0f},
    .size     = ImVec2{200.0f, 500.0f}
};
} // anonymous namespace

namespace nc::ui::editor
{
void EditorUI::Draw(Registry* registry, float*)
{
    auto graphGeometry = WindowGeometry{};
    SetNextWindowGeometry(g_graphInitialGeometry, ImGuiCond_Once);
    Window("Scene Graph", [&]()
    {
        graphGeometry = GetWindowGeometry();
        m_sceneGraph.Draw(registry);
    });

    const auto selectedEntity = m_sceneGraph.GetSelectedEntity();
    if (!selectedEntity.Valid())
    {
        return;
    }

    SetNextWindowLeftAlignedTo(graphGeometry);
    Window("Inspector", [&]()
    {
        m_inspector.Draw(registry, selectedEntity);
    });
}
} // namespace nc::ui::editor
