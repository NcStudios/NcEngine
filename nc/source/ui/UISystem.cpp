#include "UISystem.h"
#include "graphics/Graphics.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

namespace nc::ui
{
    UISystem::UISystem(HWND hwnd, ::nc::graphics::Graphics * graphics)
        : 
            #ifdef NC_EDITOR_ENABLED
            m_editor{ graphics },
            #endif
            m_projectUI{ nullptr }

    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX11_Init(graphics->m_device.Get(), graphics->m_context.Get());
    }

    UISystem::~UISystem()
    {
        ImGui_ImplWin32_Shutdown();
        ImGui_ImplDX11_Shutdown();
        ImGui::DestroyContext();
    }

    LRESULT UISystem::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        return ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam);
    }

    void UISystem::BindProjectUI(IUI* ui)
    {
        m_projectUI = ui;
    }

    void UISystem::FrameBegin()
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void UISystem::Frame(float* dt, float frameLogicTime, std::unordered_map<::nc::EntityHandle, ::nc::Entity>& activeEntities)
    {
        m_editor.Frame(dt, frameLogicTime, activeEntities);
        if(m_projectUI)
        {
            m_projectUI->Draw();
        }
    }

    void UISystem::FrameEnd()
    {
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
} //end namespace nc::ui