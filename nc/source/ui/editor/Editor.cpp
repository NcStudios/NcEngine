#ifdef NC_EDITOR_ENABLED
#include "Editor.h"
#include "imgui/imgui.h"
#include "Ecs.h"
#include "graphics/Graphics.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "component/Collider.h"
#include "component/NetworkDispatcher.h"
#include "component/PointLight.h"
#include "component/Renderer.h"
#include "input/Input.h"
#include "time/NCTime.h"

#include "Window.h"

#include <d3d11.h>
#include <string>

namespace
{
    const auto PanelSize = ImVec2{300, 0};
    const auto Padding = 4.0f;
    const auto SubPanelWidth = PanelSize.x - Padding * 4.0f;
    const auto TitleBarHeight = 40.0f;
    const auto DefaultItemWidth = 60.0f;
    const auto MainWindowFlags = ImGuiWindowFlags_NoBackground |
                                 ImGuiWindowFlags_MenuBar |
                                 ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_NoScrollbar |
                                 ImGuiWindowFlags_NoMove;

    void DrawComponentControl(nc::ComponentBase* comp)
    {
        ImGui::Separator();
        ImGui::BeginGroup();
            ImGui::Indent();
            ImGui::Spacing();
            comp->EditorGuiElement();
            ImGui::Spacing();
        ImGui::EndGroup();
    }
}

namespace nc::ui::editor
{
    Editor::Editor(nc::graphics::Graphics * graphics)
        : m_graphics{graphics},
          m_isGuiActive{false},
          m_openState_UtilitiesPanel{true},
          m_openState_GraphicsResources{false}
    {
    }

    void Editor::Frame(float* dt, ecs::EntityMap& activeEntities)
    {
        if(input::GetKeyUp(input::KeyCode::Tilde))
            ToggleGui();
        if(!m_isGuiActive)
            return;

        auto [width, height] = Window::GetDimensions();
        if(ImGui::Begin("NcEngine Editor", nullptr, MainWindowFlags))
        {
            DrawMenu();
            ImGui::SetNextWindowPos({Padding, TitleBarHeight});
            DrawSceneGraphPanel(activeEntities);

            if(m_openState_GraphicsResources)
            {
                ImGui::SameLine();
                DrawGraphicsResourcePanel();
            }

            if(m_openState_UtilitiesPanel)
            {
                ImGui::SameLine(width - PanelSize.x - Padding);
                DrawUtilitiesPanel(dt);
            }
        }
        ImGui::End();
    }

    void Editor::ToggleGui() noexcept { m_isGuiActive = !m_isGuiActive; }
    void Editor::EnableGui() noexcept { m_isGuiActive = true; }
    void Editor::DisableGui() noexcept { m_isGuiActive = false; }
    bool Editor::IsGuiActive() const noexcept { return m_isGuiActive; }

    void Editor::DrawMenu()
    {
        if(ImGui::BeginMenuBar())
        {
            if(ImGui::BeginMenu("Window"))
            {
                ImGui::MenuItem("Framerate Data",     "F...", &m_openState_UtilitiesPanel);
                ImGui::MenuItem("Graphics Resources", "F...", &m_openState_GraphicsResources);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }

    void Editor::DrawUtilitiesPanel(float* dtMult)
    {
        if(ImGui::BeginChild("UtilityPanel", PanelSize, true))
        {
            ImGui::Text("Utilities");
            ImGui::Separator();
            ImGui::Separator();
            ImGui::Spacing();
            DrawTimingControl(dtMult);
        }
        ImGui::EndChild();
    }

    void Editor::DrawGraphicsResourcePanel()
    {
        static ImGuiTextFilter filter;

        if(ImGui::BeginChild("GraphicsResourcePanel", {500, 200}, true, ImGuiWindowFlags_HorizontalScrollbar))
        {
            ImGui::Text("Graphics Resources");
            filter.Draw("##gfxFilterId", 128.0f);
            for(auto& [id, res] : graphics::d3dresource::GraphicsResourceManager::Get().m_resources)
            {
                if(filter.PassFilter(id.c_str()))
                    ImGui::Text(id.c_str());
            }
        }
        ImGui::EndChild();
    }

    void Editor::DrawTimingControl(float* dtMult)
    {
        if(ImGui::CollapsingHeader("Frame Data"))
        {
            if(ImGui::BeginChild("FrameDataPanel", {SubPanelWidth, 100}, true))
            {
                int drawCallCount = m_graphics->GetDrawCallCount();
                float frameRate = ImGui::GetIO().Framerate;
                ImGui::DragFloat("dtX", dtMult, 0.75f, 0.0f, 5.0f, "%.05f");
                ImGui::Text("%.2f fps", frameRate);
                ImGui::Text("%.2f ms/frame", 1000.0f / frameRate);
                ImGui::Text("%u Draw Calls", drawCallCount);
            }
            ImGui::EndChild();
        }
        if(ImGui::CollapsingHeader("ImGui Style Options"))
        {
            ImGui::ShowStyleEditor();
        }
    }

    void Editor::DrawSceneGraphPanel(ecs::EntityMap& entities)
    {
        if(ImGui::BeginChild("ScenePanel", PanelSize, true))
        {
            static ImGuiTextFilter filter;
            ImGui::Text("Scene Graph");
            ImGui::Spacing();
            filter.Draw("##filterlabel", 128.0f);
            ImGui::Separator();
            ImGui::Separator();
            ImGui::Spacing();

            if(ImGui::BeginChild("EntityList"))
            {
                auto uidScope = 0u;
                for(auto& [handle, entity] : entities)
                {
                    if(!filter.PassFilter(entity.Tag.c_str()))
                        continue;
                    ImGui::PushID(++uidScope);
                    if(ImGui::CollapsingHeader(entity.Tag.c_str()))
                        DrawEntityControl(handle);
                    ImGui::PopID();
                }
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();
    }

    void Editor::DrawEntityControl(nc::EntityHandle handle)
    {   
        std::string handle_s = std::to_string(static_cast<unsigned int>(handle));
        const float spacing = 60.0f;

        ImGui::Separator();

        ImGui::BeginGroup();
            ImGui::Indent();
            ImGui::PushItemWidth(spacing);
                ImGui::Text("Entity Details");
                ImGui::Indent();
                    ImGui::Text("Tag:    ");  ImGui::SameLine();  ImGui::Text(nc::Ecs::GetEntity(handle)->Tag.c_str());
                    ImGui::Text("Handle: ");  ImGui::SameLine();  ImGui::Text(handle_s.c_str());
                ImGui::Unindent();
            ImGui::PopItemWidth();
        ImGui::EndGroup();

        DrawComponentControl(nc::Ecs::GetComponent<nc::Transform>(handle));

        nc::NetworkDispatcher* disp = nc::Ecs::GetComponent<nc::NetworkDispatcher>(handle);
        if(disp) { DrawComponentControl(disp); }

        nc::Renderer* rend = nc::Ecs::GetComponent<nc::Renderer>(handle);
        if(rend) { DrawComponentControl(rend); }

        nc::Collider* col = nc::Ecs::GetComponent<nc::Collider>(handle);
        if(col) { DrawComponentControl(col); }

        nc::PointLight* light = nc::Ecs::GetComponent<PointLight>(handle);
        if(light) { DrawComponentControl(light); }

        for(const auto& comp : nc::Ecs::GetEntity(handle)->GetUserComponents())
        {
            DrawComponentControl(comp.get());
        }

        ImGui::Separator();
    }
}
#endif