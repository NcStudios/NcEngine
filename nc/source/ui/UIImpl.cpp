#include "UIImpl.h"
#include "UI.h"
#include "debug/Utils.h"
#include "graphics/Graphics.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

namespace
{
    nc::ui::UIImpl* g_instance = nullptr;
}

namespace nc::ui
{
    /* Api Function Implementation */
    void Set(IUI* ui)
    {
        V_LOG("Registering project UI");
        IF_THROW(!g_instance, "ui::Set - No UIImpl instance set");
        g_instance->BindProjectUI(ui);
    }

    bool IsHovered()
    {
        IF_THROW(!g_instance, "ui::IsHovered - No UIImpl instance set");
        return g_instance->IsProjectUIHovered();
    }

    /* UIImpl */
    #ifdef NC_EDITOR_ENABLED
    UIImpl::UIImpl(HWND hwnd, ::nc::graphics::Graphics* graphics, const ecs::Systems& systems)
        : m_editor{graphics, systems},
          m_projectUI{nullptr}
    #else
    UIImpl::UIImpl(HWND hwnd, ::nc::graphics::Graphics* graphics)
        : m_projectUI{ nullptr }
    #endif
    {
        g_instance = this;
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX11_Init(graphics->m_device, graphics->m_context);
    }

    UIImpl::~UIImpl() noexcept
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
            throw std::runtime_error("UIImpl::IsProjectUIHovered - No project UI registered");

        return m_projectUI->IsHovered();
    }

    void UIImpl::FrameBegin()
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    #ifdef NC_EDITOR_ENABLED
    void UIImpl::Frame(float* dt, std::span<Entity*> activeEntities)
    {
        m_editor.Frame(dt, activeEntities);
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