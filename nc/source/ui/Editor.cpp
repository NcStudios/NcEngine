#ifdef NC_EDITOR_ENABLED
#include "Editor.h"
#include "imgui/imgui.h"
#include "ECS.h"
#include "graphics/Graphics.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "component/PointLight.h"
#include "component/Renderer.h"
#include "input/Input.h"
#include "time/NCTime.h"

#include <d3d11.h>
#include <string>

namespace nc::ui
{
    Editor::Editor(nc::graphics::Graphics * graphics)
        : m_isGuiActive(false), m_graphics(graphics)
    {
    }

    void Editor::Frame(float* dt, float frameLogicTime, std::unordered_map<EntityHandle, Entity>& activeEntities)
    {
        if(input::GetKeyUp(input::KeyCode::Tilde)) ToggleGui();
        if(!m_isGuiActive) return;

        int drawCallCount = m_graphics->GetDrawCallCount();

        DrawMenu();
        DrawTimingControl(dt, frameLogicTime, drawCallCount, &m_openState_FramerateData);

        graphics::d3dresource::GraphicsResourceManager::DisplayResources(&m_openState_GraphicsResources);

        DrawEntityGraphControl(activeEntities);
    }

    void Editor::ToggleGui()         noexcept { m_isGuiActive = !m_isGuiActive; }
    void Editor::EnableGui()         noexcept { m_isGuiActive = true; }
    void Editor::DisableGui()        noexcept { m_isGuiActive = false; }
    bool Editor::IsGuiActive() const noexcept { return m_isGuiActive; }


    void Editor::DrawMenu()
    {
        static auto mainBlankFlags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar;
        if(ImGui::Begin("NC Engine", nullptr, mainBlankFlags))
        {
            if(ImGui::BeginMainMenuBar())
            {
                if(ImGui::BeginMenu("File"))
                {
                    ImGui::EndMenu();
                }
                if(ImGui::BeginMenu("Edit"))
                {
                    ImGui::EndMenu();
                }
                if(ImGui::BeginMenu("Window"))
                {
                    ImGui::MenuItem("Entity Graph",       "F...", &m_openState_EntityGraph);
                    ImGui::MenuItem("Framerate Data",     "F...", &m_openState_FramerateData);
                    ImGui::MenuItem("Graphics Resources", "F...", &m_openState_GraphicsResources);
                    ImGui::MenuItem("Project Settings",   "F...", &m_openState_ProjectSettings);
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }
        }
        ImGui::End();
    }

    void Editor::DrawTimingControl(float* speed, float frameLogicTime, uint32_t drawCallCount, bool* open)
    {
        if(!(*open)) return;

        (void)frameLogicTime;

        if(ImGui::Begin("Timing Data", open))
        {
            float frameRate = ImGui::GetIO().Framerate;
            ImGui::DragFloat("dtX", speed, 0.75f, 0.0f, 5.0f, "%.05f");
            ImGui::Text("%.2f fps", frameRate);
            ImGui::Text("%.2f ms/frame", 1000.0f / frameRate);
            ImGui::Text("%u Draw Calls", drawCallCount);
        }
        ImGui::End();
    }

    void Editor::DrawEntityGraphControl(std::unordered_map<EntityHandle, Entity>& entities)
    {
        if(ImGui::Begin("Entities", &m_openState_EntityGraph))
        {
            for(auto& pair : entities)
            {
                const bool selected = *SelectedEntityIndex == pair.first;
                std::string id = pair.second.Tag + "##" + std::to_string(pair.first); //create unique labels
                if(ImGui::Selectable(id.c_str(), selected))
                {
                    if(SelectedEntityIndex == pair.first)
                    {
                        SelectedEntityIndex.reset();
                    }
                    else
                    {
                        SelectedEntityIndex = pair.first;
                    }
                }
                ImGui::Spacing();
                if(selected && SelectedEntityIndex.has_value())
                {
                    DrawInspectorControl(pair.first);
                }
            }
        }
        ImGui::End();
    }

    void Editor::DrawInspectorControl(nc::EntityHandle handle)
    {   
        std::string handle_s = std::to_string(handle);
        const float spacing = 60.0f;

        ImGui::Spacing(); 
        ImGui::Separator();  ImGui::Separator();  ImGui::Separator();
        ImGui::PushItemWidth(spacing);
            ImGui::Text("Entity");
            ImGui::Indent();
                ImGui::Text("Tag:    ");  ImGui::SameLine();  ImGui::Text(nc::ECS::GetEntity(handle)->Tag.c_str());
                ImGui::Text("Handle: ");  ImGui::SameLine();  ImGui::Text(handle_s.c_str());
            ImGui::Unindent();
        ImGui::PopItemWidth();  ImGui::Separator();

        nc::ECS::GetComponent<nc::Transform>(handle)->EditorGuiElement();

        nc::Renderer* rend = nc::ECS::GetComponent<nc::Renderer>(handle);
        if(rend) { rend->EditorGuiElement(); }

        nc::PointLight* light = nc::ECS::GetComponent<PointLight>(handle);
        if(light) { light->EditorGuiElement(); }

        for(const auto& comp : nc::ECS::GetEntity(handle)->GetUserComponents())
        {
            comp->EditorGuiElement();
        }

        ImGui::Separator();
        ImGui::Separator();
        ImGui::Spacing();
    }
}
#endif