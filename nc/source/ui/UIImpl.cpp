#include "UIImpl.h"
#include "graphics/Graphics.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

namespace nc::ui
{
    UIImpl::UIImpl(HWND hwnd, ::nc::graphics::Graphics * graphics)
        : 
            #ifdef NC_EDITOR_ENABLED
            m_editor{ graphics },
            #endif
            m_projectUI{ nullptr }
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX11_Init(graphics->m_device, graphics->m_context);
    }

    UIImpl::~UIImpl()
    {
        ImGui_ImplWin32_Shutdown();
        ImGui_ImplDX11_Shutdown();
        ImGui::DestroyContext();
    }

    LRESULT UIImpl::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        return ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam);
    }

    void UIImpl::BindProjectUI(IUI* ui)
    {
        m_projectUI = ui;
    }

    bool UIImpl::IsProjectUIHovered()
    {
        if (!m_projectUI)
        {
            throw std::runtime_error("No project UI registered");
        }
        return m_projectUI->IsHovered();
    }

    void UIImpl::FrameBegin()
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    #ifdef NC_EDITOR_ENABLED
    void UIImpl::Frame(float* dt, float frameLogicTime, ecs::EntityMap& activeEntities)
    {
        m_editor.Frame(dt, frameLogicTime, activeEntities);
        if(m_projectUI)
        {
            m_projectUI->Draw();
        }
    }
    #else
    void UIImpl::Frame()
    {
        if(m_projectUI)
        {
            m_projectUI->Draw();
        }
    }
    #endif

    void UIImpl::FrameEnd()
    {
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
} //end namespace nc::ui