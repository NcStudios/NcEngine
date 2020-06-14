#ifdef NC_EDITOR_ENABLED
#include "EditorManager.h"
#include <d3d11.h>
#include "external/imgui/imgui.h"
#include "external/imgui/imgui_impl_win32.h"
#include "external/imgui/imgui_impl_dx11.h"
#include "graphics/Graphics.h"
#include "component/Transform.h"
#include "Entity.h"
#include "component/PointLight.h"
#include "input/Input.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "time/NCTime.h"
#include "NCE.h"

#include "component/Renderer.h" //prob not needed


#include <string>


namespace nc::utils::editor
{
    const ImVec4 COLOR_DEFAULT_TEXT   (0.8f,   1.0f,   0.88f,  1.0f);
    const ImVec4 COLOR_DEFAULT_BG     (0.1f,   0.3f,   0.225f, 1.0f);
    const ImVec4 COLOR_DEFAULT_ACTIVE (0.225f, 0.714f, 0.6f,   1.0f);
    const ImVec4 COLOR_DEFAULT_HOVERED(0.125f, 0.506f, 0.40f,  1.0f);
    const ImVec4 COLOR_DEFAULT_ITEM   (0.1f,   0.404f, 0.325f, 1.0f);

    //const ImVec4 COLOR_DEFAULT_  (0.137f, 0.508f, 0.388f, 1.0f);
    //const ImVec4 COLOR_DEFAULT_  (0.408f, 0.716f,   0.6f, 1.0f);

    EditorManager::EditorManager(HWND hwnd, nc::graphics::Graphics * graphics)
        : m_isGuiActive(false), m_graphics(graphics)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX11_Init(graphics->m_device.Get(), graphics->m_context.Get());
        auto& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigDockingWithShift = false;
        auto& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_Text]                 = COLOR_DEFAULT_TEXT;
        style.Colors[ImGuiCol_Separator]            = COLOR_DEFAULT_ACTIVE;
        style.Colors[ImGuiCol_MenuBarBg]            = COLOR_DEFAULT_BG;
        style.Colors[ImGuiCol_TitleBg]              = COLOR_DEFAULT_BG;
        style.Colors[ImGuiCol_TitleBgCollapsed]     = COLOR_DEFAULT_BG;
        style.Colors[ImGuiCol_ScrollbarBg]          = COLOR_DEFAULT_BG;
        style.Colors[ImGuiCol_FrameBg]              = COLOR_DEFAULT_BG;
        style.Colors[ImGuiCol_TabUnfocused]         = COLOR_DEFAULT_BG;
        style.Colors[ImGuiCol_TabUnfocusedActive]   = COLOR_DEFAULT_ITEM;
        style.Colors[ImGuiCol_ScrollbarGrab]        = COLOR_DEFAULT_ITEM;
        style.Colors[ImGuiCol_SliderGrab]           = COLOR_DEFAULT_ITEM;
        style.Colors[ImGuiCol_Header]               = COLOR_DEFAULT_ITEM;
        style.Colors[ImGuiCol_ResizeGrip]           = COLOR_DEFAULT_ITEM;
        style.Colors[ImGuiCol_Tab]                  = COLOR_DEFAULT_ITEM;
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = COLOR_DEFAULT_HOVERED;
        style.Colors[ImGuiCol_HeaderHovered]        = COLOR_DEFAULT_HOVERED;
        style.Colors[ImGuiCol_FrameBgHovered]       = COLOR_DEFAULT_HOVERED;
        style.Colors[ImGuiCol_ResizeGripHovered]    = COLOR_DEFAULT_HOVERED;
        style.Colors[ImGuiCol_TabHovered]           = COLOR_DEFAULT_HOVERED;
        style.Colors[ImGuiCol_TitleBgActive]        = COLOR_DEFAULT_HOVERED;
        style.Colors[ImGuiCol_ScrollbarGrabActive]  = COLOR_DEFAULT_ACTIVE;
        style.Colors[ImGuiCol_SliderGrabActive]     = COLOR_DEFAULT_ACTIVE;
        style.Colors[ImGuiCol_FrameBgActive]        = COLOR_DEFAULT_ACTIVE;
        style.Colors[ImGuiCol_HeaderActive]         = COLOR_DEFAULT_ACTIVE;
        style.Colors[ImGuiCol_ResizeGripActive]     = COLOR_DEFAULT_ACTIVE;
        style.Colors[ImGuiCol_TabActive]            = COLOR_DEFAULT_ACTIVE;
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

    void EditorManager::Frame(float* dt, float frameLogicTime, std::unordered_map<EntityHandle, Entity>& activeEntities)
    {
        if(!m_isGuiActive) return;

        int drawCallCount = m_graphics->GetDrawCallCount();

        DrawMenu();
        DrawTimingControl(dt, frameLogicTime, drawCallCount, &m_openState_FramerateData);

        graphics::d3dresource::GraphicsResourceManager::DisplayResources(&m_openState_GraphicsResources);

        DrawEntityGraphControl(activeEntities);
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

    void EditorManager::DrawTimingControl(float* speed, float frameLogicTime, uint32_t drawCallCount, bool* open)
    {
        if(!(*open)) return;

        (void)frameLogicTime;

        //if(ImGui::Begin("Timing Data", open, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDecoration))
        if(ImGui::Begin("Timing Data", open))
        {
            float frameRate = ImGui::GetIO().Framerate;
            ImGui::DragFloat("dtX", speed, 0.75f, 0.0f, 5.0f, "%.05f");
            ImGui::Text("%.2f fps", frameRate);
            ImGui::Text("%.2f ms/frame", 1000.0f / frameRate);
            //ImGui::Text("%.2f ns/update", frameLogicTime);
            ImGui::Text("%u Draw Calls", drawCallCount);
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
                    DrawInspectorControl(pair.first);
                }
            }
        }
        ImGui::End();
    }

    void EditorManager::DrawInspectorControl(nc::EntityHandle handle)
    {   
        std::string handle_s = std::to_string(handle);
        const float spacing = 60.0f;

        ImGui::Spacing(); 
        ImGui::Separator();  ImGui::Separator();  ImGui::Separator();
        ImGui::PushItemWidth(spacing);
            ImGui::Text("Entity");
            ImGui::Indent();
                ImGui::Text("Tag:    ");  ImGui::SameLine();  ImGui::Text(NCE::GetEntity(handle)->Tag.c_str());
                ImGui::Text("Handle: ");  ImGui::SameLine();  ImGui::Text(handle_s.c_str());
            ImGui::Unindent();
        ImGui::PopItemWidth();  ImGui::Separator();

        NCE::GetEngineComponent<nc::Transform>(handle)->EditorGuiElement();

        nc::Renderer* rend = NCE::GetEngineComponent<nc::Renderer>(handle);
        if(rend) { rend->EditorGuiElement(); }

        nc::PointLight* light = NCE::GetEngineComponent<PointLight>(handle);
        if(light) { light->EditorGuiElement(); }

        for(const auto& comp : NCE::GetEntity(handle)->GetUserComponents())
        {
            comp->EditorGuiElement();
        }

        ImGui::Separator();
        ImGui::Separator();
        ImGui::Spacing();
    }
}
#endif