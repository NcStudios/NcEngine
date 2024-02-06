#include "EditorUI.h"
#include "ui/editor/Editor.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/input/Input.h"
#include "ncengine/scene/NcScene.h"
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
EditorUI::EditorUI(ecs::Ecs world, ModuleProvider modules)
    : m_createEntityDialog{world},
      m_newSceneDialog{modules.Get<NcScene>()},
      m_saveSceneDialog{world},
      m_loadSceneDialog{world, modules.Get<NcScene>()}
{
}

void EditorUI::Draw(EditorContext& ctx)
{
    auto& ncAsset = *ctx.modules.Get<asset::NcAsset>();
    ctx.dimensions = ImVec2{window::GetDimensions()};
    DrawOverlays(ctx.dimensions);
    switch (ctx.openState = ProcessInput(ctx.hotkeys, ncAsset))
    {
        case OpenState::ClosePersisted: { return; }
        case OpenState::Closed:
        {
            m_sceneGraph.OnClose(ctx);
            return;
        }
    }

    DrawDialogs(ctx);

    RUN_ONCE(WindowLayout(g_initialGraphWidth, g_pivotLeft));
    Window("Scene Graph", ImGuiWindowFlags_MenuBar, [&]()
    {
        DrawMenu(ncAsset);
        m_sceneGraph.Draw(ctx, m_createEntityDialog);
    });

    if (!ctx.selectedEntity.Valid())
    {
        return;
    }

    RUN_ONCE(WindowLayout(g_initialInspectorWidth, g_pivotRight));
    Window("Inspector", [&]()
    {
        m_inspector.Draw(ctx, m_createEntityDialog);
    });
}

auto EditorUI::ProcessInput(const EditorHotkeys& hotkeys, asset::NcAsset& ncAsset) -> OpenState
{
    auto state = OpenState::ClosePersisted;
    if(KeyDown(hotkeys.toggleEditor))
    {
        m_open = !m_open;
        state = m_open ? OpenState::Opened : OpenState::Closed;
    }
    else if (m_open)
    {
        state = OpenState::OpenPersisted;
    }

    if (!m_open)
        return state;

    if (KeyDown(hotkeys.openNewSceneDialog))
        m_newSceneDialog.Open();
    else if (KeyDown(hotkeys.openSaveSceneDialog))
        m_saveSceneDialog.Open(ncAsset.GetLoadedAssets());
    else if (KeyDown(hotkeys.openLoadSceneDialog))
        m_loadSceneDialog.Open(&ncAsset);

    return state;
}

void EditorUI::DrawOverlays(const ImVec2& dimensions)
{
    if (m_fpsOverlay.IsOpen())
        m_fpsOverlay.Draw(dimensions);
}

void EditorUI::DrawDialogs(EditorContext& ctx)
{
    if (m_createEntityDialog.IsOpen())
        m_createEntityDialog.Draw(ctx.dimensions, ctx.selectedEntity);
    else if (m_newSceneDialog.IsOpen())
        m_newSceneDialog.Draw(ctx);
    else if (m_saveSceneDialog.IsOpen())
        m_saveSceneDialog.Draw(ctx.dimensions);
    else if (m_loadSceneDialog.IsOpen())
        m_loadSceneDialog.Draw(ctx);
}

void EditorUI::DrawMenu(asset::NcAsset& ncAsset)
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Scene"))
        {
            if (ImGui::MenuItem("New"))
                m_newSceneDialog.Open();
            if (ImGui::MenuItem("Save"))
                m_saveSceneDialog.Open(ncAsset.GetLoadedAssets());
            if (ImGui::MenuItem("Load"))
                m_loadSceneDialog.Open(&ncAsset);

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Debug"))
        {
            if (ImGui::MenuItem("FPS Overlay"))
                m_fpsOverlay.ToggleOpen();

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}
} // namespace nc::ui::editor
