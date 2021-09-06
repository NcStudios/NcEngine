#include "UIImpl.h"
#include "UI.h"
#include "debug/Utils.h"
#include "graphics/Graphics2.h"
#include "graphics/Base.h"
#include "graphics/Swapchain.h"
#include "imgui/imgui_impl_vulkan.h"
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
    UIImpl::UIImpl(HWND hwnd, ::nc::graphics::Graphics2* graphics)
    : m_editor{graphics},
      m_projectUI{nullptr},
      m_graphics{graphics}
    #else
    UIImpl::UIImpl(HWND hwnd, ::nc::graphics::Graphics2* graphics)
    : m_projectUI{nullptr},
      m_graphics{graphics}
    #endif
    {
        g_instance = this;
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hwnd);

        auto& uiPassDefinition = m_graphics->GetSwapchainPtr()->GetPassDefinition();
        m_graphics->GetBasePtr()->InitializeImgui(uiPassDefinition);
    }

    UIImpl::~UIImpl() noexcept
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplWin32_Shutdown();
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
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    #ifdef NC_EDITOR_ENABLED
    void UIImpl::Frame(float* dt, registry_type* registry)
    {
        m_editor.Frame(dt, registry);
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
    }
} //end namespace nc::ui