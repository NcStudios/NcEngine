#include "EditorManager.h"
#include <d3d11.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "Graphics.h"
#include "Transform.h"
#include "EntityView.h"
#include "Entity.h"
#include "PointLight.h"
#include "Input.h"
#include "GraphicsResourceManager.h"

#include "Renderer.h" //prob not needed


#include <string>


namespace nc::utils::editor
{
    const ImVec4 COLOR_TEAL_GRAY      (  0.0f, 0.102f, 0.094f, 1.0f);
    const ImVec4 COLOR_TEAL_TEXT      (  0.6f,   1.0f,  0.98f, 1.0f);

    const ImVec4 COLOR_TEAL_LIGHTEST  (0.137f, 0.408f, 0.388f, 1.0f);
    const ImVec4 COLOR_TEAL_LIGHT     (0.408f, 0.616f,   0.6f, 1.0f);
    const ImVec4 COLOR_TEAL_NORMAL    (0.255f, 0.514f, 0.494f, 1.0f);
    const ImVec4 COLOR_TEAL_DARK      (0.051f, 0.306f,  0.29f, 1.0f);
    const ImVec4 COLOR_TEAL_DARKEST   (  0.0f, 0.204f, 0.188f, 1.0f);
    const ImVec4 COLOR_BLUE_NORMAL    (0.306f, 0.388f, 0.577f, 1.0f);
    const ImVec4 COLOR_BLUE_DARK      (0.082f, 0.169f, 0.333f, 1.0f);
    const ImVec4 COLOR_BLUE_DARKEST   (0.024f,  0.09f, 0.224f, 1.0f);
    const ImVec4 COLOR_GREEN_LIGHTEST (0.216f, 0.545f,  0.18f, 1.0f);
    const ImVec4 COLOR_GREEN_LIGHT    (0.573f,  0.82f, 0.545f, 1.0f);
    const ImVec4 COLOR_GREEN_NORMAL   (0.373f, 0.682f, 0.341f, 1.0f);
    const ImVec4 COLOR_GREEN_DARK     (0.102f, 0.408f, 0.067f, 1.0f);
    const ImVec4 COLOR_GREEN_DARKEST  (0.027f, 0.275f,   0.0f, 1.0f);


    EditorManager::EditorManager(HWND hwnd, nc::graphics::Graphics& graphics)
        : m_isGuiActive(false)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX11_Init(graphics.m_device.Get(), graphics.m_context.Get());
    }

    EditorManager::~EditorManager() noexcept
    {
        ImGui_ImplWin32_Shutdown();
        ImGui_ImplDX11_Shutdown();
        ImGui::DestroyContext();
    }

    LRESULT EditorManager::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        return ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam);
    }

    void EditorManager::BeginFrame()
    {
        if(input::GetKeyUp(input::KeyCode::Tilde)) ToggleGui();
        if(!m_isGuiActive) return;
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void EditorManager::Frame(float* dt, std::unordered_map<EntityHandle, Entity>& activeEntities)
    {
        if(!m_isGuiActive) return;

        ImGui::PushStyleColor(ImGuiCol_Text,                 COLOR_TEAL_TEXT);

        ImGui::PushStyleColor(ImGuiCol_MenuBarBg,            COLOR_TEAL_DARKEST);

        ImGui::PushStyleColor(ImGuiCol_TitleBg,              COLOR_TEAL_GRAY);
        ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed,     COLOR_TEAL_GRAY);
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive,        COLOR_TEAL_DARKEST);

        ImGui::PushStyleColor(ImGuiCol_ScrollbarBg,          COLOR_TEAL_DARK);
        ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab,        COLOR_TEAL_NORMAL);
        ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, COLOR_TEAL_LIGHTEST);
        ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive,  COLOR_TEAL_LIGHTEST);

        ImGui::PushStyleColor(ImGuiCol_SliderGrab,           COLOR_TEAL_DARKEST);
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive,     COLOR_TEAL_DARK);

        ImGui::PushStyleColor(ImGuiCol_FrameBg,              COLOR_TEAL_DARKEST);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,       COLOR_TEAL_DARK);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive,        COLOR_TEAL_NORMAL);

        ImGui::PushStyleColor(ImGuiCol_Header,               COLOR_TEAL_DARKEST);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered,        COLOR_TEAL_DARK);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive,         COLOR_TEAL_NORMAL);

        ImGui::PushStyleColor(ImGuiCol_ResizeGrip,           COLOR_TEAL_DARKEST);
        ImGui::PushStyleColor(ImGuiCol_ResizeGripHovered,    COLOR_TEAL_DARK);
        ImGui::PushStyleColor(ImGuiCol_ResizeGripActive,     COLOR_TEAL_NORMAL);

        ImGui::PushStyleColor(ImGuiCol_Separator,            COLOR_TEAL_LIGHTEST);

        DrawMenu();
        DrawFrameControl(dt, &m_openState_FramerateData);

        graphics::d3dresource::GraphicsResourceManager::DisplayResources(&m_openState_GraphicsResources);

        DrawEntityGraphControl(activeEntities);

        ImGui::PopStyleColor(21);
    }

    void EditorManager::EndFrame()
    {
        if(!m_isGuiActive) return;
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    void EditorManager::ToggleGui()         noexcept { m_isGuiActive = !m_isGuiActive; }
    void EditorManager::EnableGui()         noexcept { m_isGuiActive = true; }
    void EditorManager::DisableGui()        noexcept { m_isGuiActive = false; }
    bool EditorManager::IsGuiActive() const noexcept { return m_isGuiActive; }


    void EditorManager::DrawMenu()
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

    void EditorManager::DrawFrameControl(float* speed, bool* open)
    {
        if(!(*open)) return;

        if(ImGui::Begin("Frame dt", open, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDecoration))
        {
            float frameRate = ImGui::GetIO().Framerate;
            ImGui::DragFloat("dtX", speed, 0.75f, 0.0f, 5.0f, "%.05f");
            ImGui::Text("Avg. %.1f FPS", frameRate);
            ImGui::Text("(%.3f ms/frame)", 1000.0f / frameRate);
        }
        ImGui::End();
    }

    void EditorManager::DrawEntityGraphControl(std::unordered_map<EntityHandle, Entity>& entities)
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
                    EntityView view(pair.second.Handle, pair.second.TransformHandle); 
                    DrawInspectorControl(&view);
                }
            }
        }
        ImGui::End();
    }

    void EditorManager::DrawInspectorControl(nc::EntityView* view)
    {   
        std::string handle_s = std::to_string(view->Entity()->Handle);
        const float spacing = 60.0f;

        ImGui::Spacing(); 
        ImGui::Separator();  ImGui::Separator();  ImGui::Separator();
        ImGui::PushItemWidth(spacing);
            ImGui::Text("Entity");
            ImGui::Indent();
                ImGui::Text("Tag:    ");  ImGui::SameLine();  ImGui::Text(view->Entity()->Tag.c_str());
                ImGui::Text("Handle: ");  ImGui::SameLine();  ImGui::Text(handle_s.c_str());
            ImGui::Unindent();
        ImGui::PopItemWidth();  ImGui::Separator();

        view->Transform()->EditorGuiElement();

        nc::Renderer* rend = view->Renderer();
        if(rend) { rend->EditorGuiElement(); }

        nc::PointLight* light = view->GetPointLight();
        if(light) { light->EditorGuiElement(); }

        for(auto& comp : view->Entity()->GetUserComponents())
        {
            comp->EditorGuiElement();
        }

        ImGui::Separator();
        ImGui::Separator();
        ImGui::Spacing();
    }
}