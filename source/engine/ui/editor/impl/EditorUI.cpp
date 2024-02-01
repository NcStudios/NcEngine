#include "EditorUI.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/ui/ImGuiUtility.h"
#include "ncengine/window/Window.h"

// Helper for setting up initial window layout:
// Runs a block only the first time its seen by wrapping in an immediately
// invoked lambda and storing an arbitrary result in a local static.
#define RUN_ONCE(f) { [[maybe_unused]] static auto _ = [](){f; return true;}(); }

namespace
{
constexpr auto g_initialGraphWidth = 180.0f;
constexpr auto g_initialInspectorWidth = 240.0f;
constexpr auto g_pivotLeft = ImVec2{0.0f, 0.0f};
constexpr auto g_pivotRight = ImVec2{1.0f, 0.0f};

void WindowLayout(float width, ImVec2 pivot)
{
    const auto windowDimensions = nc::window::GetDimensions();
    const auto screenExtent = nc::window::GetScreenExtent();
    const auto [xPadding, yPadding] = (windowDimensions - screenExtent) / 2.0f;
    const auto size = ImVec2{width, screenExtent.y / 2.0f};
    const auto pos = ImVec2{xPadding + screenExtent.x * pivot.x - width * pivot.x, yPadding};
    ImGui::SetNextWindowSize(size, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(pos, ImGuiCond_FirstUseEver);
}
} // anonymous namespace

namespace nc::ui::editor
{
void EditorUI::Draw(ecs::Ecs world)
{
    if (m_createEntityWindow.IsOpen())
        m_createEntityWindow.Draw(world, m_sceneGraph.GetSelectedEntity());

    RUN_ONCE(WindowLayout(g_initialGraphWidth, g_pivotLeft));
    Window("Scene Graph", [&]()
    {
        m_sceneGraph.Draw(world, m_createEntityWindow);
    });

    const auto selectedEntity = m_sceneGraph.GetSelectedEntity();
    if (!selectedEntity.Valid())
    {
        return;
    }

    RUN_ONCE(WindowLayout(g_initialInspectorWidth, g_pivotRight));
    Window("Inspector", [&]()
    {
        m_inspector.Draw(world, selectedEntity, m_createEntityWindow);
    });
}
} // namespace nc::ui::editor
