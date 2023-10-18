#include "EditorUI.h"
#include "ncengine/ecs/Registry.h"

#include "imgui/imgui.h"

namespace
{
constexpr auto g_graphInitialPosition = ImVec2{0.0f, 0.0f};
constexpr auto g_graphInitialSize = ImVec2{200.0f, 500.0f};
constexpr auto g_dragRegionPadding = 40.0f;

struct WindowLayout
{
    ImVec2 position;
    ImVec2 size;
};

void CaptureLayout(WindowLayout& out)
{
    out.position = ImGui::GetWindowPos();
    out.size = ImGui::GetWindowSize();
}

// void SetWindowAdjacentTo(const ImVec2& targetPos, const ImVec2& targetSize)
void SetWindowAdjacentTo(const WindowLayout& layout)
{
    ImGui::SameLine();
    ImGui::SetNextWindowPos(ImVec2{layout.position.x + layout.size.x, layout.position.y});
    // ImGui::SetNextWindowPos(ImVec2{targetPos.x + targetSize.x, targetPos.y});
}


template<class F>
void Window(const char* label, F&& func)
{
    if (ImGui::Begin(label, nullptr))
    {
        func();
    }

    ImGui::End();
}

template<class F>
void Window(const char* label, const WindowLayout& layout, F&& func)
{
    ImGui::SetNextWindowPos(layout.position, ImGuiCond_Once);
    ImGui::SetNextWindowSize(layout.size, ImGuiCond_Once);
    if (ImGui::Begin(label, nullptr))
    {
        func();
    }

    ImGui::End();
}
} // anonymous namespace

namespace nc::ui::editor
{
void EditorUI::Draw(Registry* registry, float*)
{
    auto layout = WindowLayout{};
    Window("Scene Graph", {g_graphInitialPosition, g_graphInitialSize}, [&]()
    {
        CaptureLayout(layout);
        m_sceneGraph.Draw(registry);
    });

    const auto selectedEntity = m_sceneGraph.GetSelectedEntity();
    if (!selectedEntity.Valid())
    {
        return;
    }

    SetWindowAdjacentTo(layout);
    Window("Inspector", [&]()
    {
        m_inspector.Draw(registry, selectedEntity);
    });



    // ImGui::SetNextWindowPos(g_graphInitialPosition, ImGuiCond_Once);
    // ImGui::SetNextWindowSize(g_graphInitialSize, ImGuiCond_Once);
    // auto graphPosition = ImVec2{};
    // auto graphSize = ImVec2{};

    // if (ImGui::Begin("Scene Graph", nullptr))
    // {
    //     graphPosition = ImGui::GetWindowPos();
    //     graphSize = ImGui::GetWindowSize();
    //     m_sceneGraph.Draw(registry);
    // }

    // ImGui::End();

    // const auto selectedEntity = m_sceneGraph.GetSelectedEntity();
    // if (!selectedEntity.Valid())
    // {
    //     return;
    // }

    // SetWindowAdjacentTo(graphPosition, graphSize);
    // if (ImGui::Begin("Inspector", nullptr))
    // {
    //     m_inspector.Draw(registry, selectedEntity);
    // }

    // ImGui::End();
}
} // namespace nc::ui::editor
