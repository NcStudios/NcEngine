#include "EditorUI.h"
#include "UIStyle.h"

#include "imgui/imgui.h"
#include "ncengine/window/Window.h"

namespace
{
constexpr auto g_mainWindowFlags = ImGuiWindowFlags_NoBackground |
                                   ImGuiWindowFlags_NoTitleBar |
                                   ImGuiWindowFlags_MenuBar |
                                   ImGuiWindowFlags_NoResize |
                                   ImGuiWindowFlags_NoScrollbar |
                                   ImGuiWindowFlags_NoMove;
} // anonymous namespace

namespace nc::editor
{
EditorUI::EditorUI()
    : m_dimensions{window::GetDimensions()}
{
    SetImGuiStyle();
}

EditorUI::~EditorUI() noexcept = default;

void EditorUI::Draw()
{
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize({m_dimensions.x, 20.0f});

    if (ImGui::Begin("NcEditor", nullptr, g_mainWindowFlags))
    {
    }

    ImGui::End();
}

void EditorUI::OnResize(Vector2 dimensions)
{
    m_dimensions = dimensions;
}
} // namespace nc::editor
