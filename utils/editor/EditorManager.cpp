#include "EditorManager.h"
#include <d3d11.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "Graphics.h"

#include <string>


namespace nc::editor
{
    EditorManager::EditorManager(HWND hwnd, nc::graphics::internal::Graphics& graphics)
        : m_isGuiActive(false)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        //setup win32 bindings
        ImGui_ImplWin32_Init(hwnd);

        //setup D3D11 bindings
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

    void EditorManager::ToggleGui() noexcept { m_isGuiActive = !m_isGuiActive; }
    void EditorManager::EnableGui() noexcept { m_isGuiActive = true; }

    void EditorManager::DisableGui() noexcept { m_isGuiActive = false; }

    bool EditorManager::IsGuiActive() const noexcept { return m_isGuiActive; }

    void EditorManager::SpeedControl(float* speed)
    {
        if(ImGui::Begin("Simulation Speed"))
        {
            ImGui::SliderFloat("Factor", speed, 0.0f, 10.0f);
            ImGui::Text("Average %.1f FPS (%.3f ms/frame)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
        }
        ImGui::End();
    }

    void EditorManager::CameraControl(float* zPos, float* phi, float* theta, float* xRot, float* yRot, float* zRot)
    {
        if(ImGui::Begin("Camera Control"))
        {
            ImGui::Text("Position");
            ImGui::SliderFloat("Z Dist", zPos, 0.0f, 100.0f, "%.1f");
            ImGui::SliderAngle("Phi", phi, -180.0f, 180.0f, "%.1f");
            ImGui::SliderAngle("Theta", theta, -180.0f, 180.0f, "%.1f");
            ImGui::Text("Rotation");
            ImGui::SliderAngle("X Angle", xRot, -180.0f, 180.0f);
            ImGui::SliderAngle("Y Angle", yRot, -180.0f, 180.0f);
            ImGui::SliderAngle("Z Angle", zRot, -180.0f, 180.0f);
            if(ImGui::Button("Reset")) { *zPos=20.0f;*phi=0;*theta=0;*xRot=0;*yRot=0;*zRot=0; }
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

    
    bool EditorManager::BoxControl(int id, float* matX, float* specInten, float* specPwr,
                                   float* r, float* theta, float* phi)
    {
        using namespace std::string_literals;

        bool isDirty = false;
        if(ImGui::Begin("Box "))// + std::to_string(id).c_str() ) )
        {
            bool mDirty  = ImGui::ColorEdit3("Material Color", matX);
            bool siDirty = ImGui::SliderFloat("Specular Intensity", specInten, 0.05f, 4.0f, "%.2f", 2);
            bool spDirty = ImGui::SliderFloat("Specular Power", specPwr, 1.0f, 200.0f, "%.2f", 2);
            isDirty = mDirty || siDirty || spDirty;

            ImGui::Text("Position");
            ImGui::SliderFloat("R", r, 0.0f, 80.0f, "%.1f");
            ImGui::SliderAngle("Theta", theta, -180.0f, 180.0f);
            ImGui::SliderAngle("Phi", phi, -180.0f, 180.0f);
        }
        ImGui::End();

        return isDirty;
    }

}