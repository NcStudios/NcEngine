#include "EditorManager.h"
#include <d3d11.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "Graphics.h"
#include "Transform.h"
#include "EntityView.h"
#include "Entity.h"


#include "Renderer.h" //prob not needed


#include <string>


namespace nc::utils::editor
{
    EditorManager::EditorManager(HWND hwnd, nc::graphics::Graphics& graphics)
        : m_isGuiActive(false)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        //ImGui::StyleColorsDark();
        ImGui::StyleColorsClassic();

        //setup win32/d3d11 bindings
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
        if(!m_isGuiActive) return;
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void EditorManager::EndFrame()
    {
        if(!m_isGuiActive) return;
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    void EditorManager::ToggleGui()  noexcept { m_isGuiActive = !m_isGuiActive; }
    void EditorManager::EnableGui()  noexcept { m_isGuiActive = true; }
    void EditorManager::DisableGui() noexcept { m_isGuiActive = false; }

    bool EditorManager::IsGuiActive() const noexcept { return m_isGuiActive; }



    bool EditorManager::EntityControl(nc::EntityView* view)
    {   
        std::string handle_s = std::to_string(view->Entity()->Handle);
        float spacing = 60.0f;
        static bool open = true;

        if(open) 
        {
            if(ImGui::Begin("Entity"), &open)
            {
                ImGui::Spacing(); ImGui::Separator();  ImGui::PushItemWidth(spacing);
                    ImGui::BeginGroup();
                        ImGui::Text("Tag: ");  ImGui::SameLine(spacing, -1.0f);  ImGui::Text(view->Entity()->Tag.c_str());
                    ImGui::EndGroup();
                    ImGui::BeginGroup();
                        ImGui::Text("Handle: ");  ImGui::SameLine(spacing, -1.0f);  ImGui::Text(handle_s.c_str());
                    ImGui::EndGroup();
                ImGui::PopItemWidth();  ImGui::Separator();

                view->Transform()->EditorGuiElement();

                nc::Renderer* rend = view->Renderer();
                if(rend) { rend->EditorGuiElement(); }

                for(auto& comp : view->Entity()->GetUserComponents())
                {
                    comp->EditorGuiElement();
                }
            }
            ImGui::End();
        }

        return open;
    }


    void EditorManager::SpeedControl(float* speed)
    {
        static bool open = true;

        if(!open) return;

        if(ImGui::Begin("Frame dt"), &open)
        {
            ImGui::DragFloat("Mult", speed, 0.75f, 0.0f, 10.0f, "%.05f");
            float frameRate = ImGui::GetIO().Framerate;
            ImGui::Text("Avg. %.1f FPS", frameRate);
            ImGui::Text("(%.3f ms/frame)", 1000.0f / frameRate);
        }
        ImGui::End();
    }

    void EditorManager::PointLightControl(float* xPos,      float* yPos,      float* zPos,
                                          float* ambientR,  float* ambientG,  float* ambientB,
                                          float* difColorR, float* difColorG, float* difColorB,
                                          float* difIntens, float* attConst,  float* attLin, float* attQuad)
    {
        if(ImGui::Begin("Point Light Control"))
        {
            ImGui::Text("Position");
            ImGui::SliderFloat("X", xPos, -60.0f, 60.0f, "%.1f");
            ImGui::SliderFloat("Y", yPos, -60.0f, 60.0f, "%.1f");
            ImGui::SliderFloat("Z", zPos, -60.0f, 60.0f, "%.1f");
            
            ImGui::Text("Color");
            ImGui::ColorEdit3("Ambient", ambientR);
            ImGui::ColorEdit3("Diffuse", difColorR);
            ImGui::SliderFloat("Diffuse Intensity", difIntens, 0.01f, 2.0f, "%.05f", 2);

            ImGui::Text("Attenuation");
            ImGui::SliderFloat("Constant", attConst, 0.05f, 10.0f, "%.2f", 4);
            ImGui::SliderFloat("Linear", attLin, 0.0001f, 4.0f, "%.4f", 8);
            ImGui::SliderFloat("Quadratic", attQuad, 0.0000001f, 10.0f, "%.7f", 10);
        }
        ImGui::End();
    }
}