#include "EditorUI.h"
#include "ui/editor/Editor.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/input/Input.h"
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
EditorUI::EditorUI(ecs::Ecs world)
    : m_newSceneDialog{},
      m_saveSceneDialog{world},
      m_loadSceneDialog{world}
{
}

void EditorUI::Draw(const EditorHotkeys& hotkeys, ecs::Ecs world, asset::NcAsset& ncAsset)
{

    if(input::KeyDown(hotkeys.toggleEditor))
        m_open = !m_open;

    const auto dimensions = window::GetDimensions();
    if (m_fpsOverlay.IsOpen()) m_fpsOverlay.Draw(ImVec2{dimensions.x, dimensions.y});

    if(!m_open)
        return;

    RUN_ONCE(WindowLayout(g_initialGraphWidth, g_pivotLeft));
    Window("Scene Graph", [&]()
    {
        // if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("FPS"))
                {
                    m_fpsOverlay.ToggleOpen();
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Scene"))
            {
                if (ImGui::MenuItem("New")) m_newSceneDialog.Open();
                if (ImGui::MenuItem("Save")) m_saveSceneDialog.Open(ncAsset.GetLoadedAssets());
                if (ImGui::MenuItem("Load")) m_loadSceneDialog.Open(&ncAsset);
                ImGui::EndMenu();
            }

            // ImGui::EndMenuBar();
        }

        m_sceneGraph.Draw(world);
    });

    if (KeyDown(hotkeys.openLoadSceneDialog)) m_newSceneDialog.Open();
    else if (KeyDown(hotkeys.openSaveSceneDialog)) m_saveSceneDialog.Open(ncAsset.GetLoadedAssets());
    else if (KeyDown(hotkeys.openLoadSceneDialog)) m_loadSceneDialog.Open(&ncAsset);


    if (m_newSceneDialog.IsOpen()) m_newSceneDialog.Draw();
    else if (m_saveSceneDialog.IsOpen()) m_saveSceneDialog.Draw();
    else if (m_loadSceneDialog.IsOpen()) m_loadSceneDialog.Draw();

    const auto selectedEntity = m_sceneGraph.GetSelectedEntity();
    if (!selectedEntity.Valid())
    {
        return;
    }

    RUN_ONCE(WindowLayout(g_initialInspectorWidth, g_pivotRight));
    Window("Inspector", [&]()
    {
        m_inspector.Draw(world, selectedEntity);
    });
}
} // namespace nc::ui::editor
