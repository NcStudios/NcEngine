#include "UISystemImpl.h"
#include "debug/Utils.h"
#include "graphics/Base.h"
#include "graphics/graphics_interface.h"
#include "graphics/Swapchain.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_vulkan.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

namespace nc::ui
{
    #ifdef NC_EDITOR_ENABLED
    UISystemImpl::UISystemImpl(HWND hwnd, graphics::graphics_interface* graphics)
    : m_editor{},
      m_projectUI{nullptr},
      m_graphics{graphics}
    #else
    UISystemImpl::UISystemImpl(HWND hwnd, graphics::graphics_interface* graphics)
    : m_projectUI{nullptr},
      m_graphics{graphics}
    #endif
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hwnd);

        m_graphics->initialize_ui();
    }

    UISystemImpl::~UISystemImpl() noexcept
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    LRESULT UISystemImpl::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        return ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam);
    }

    void UISystemImpl::Set(IUI* ui) noexcept
    {
        m_projectUI = ui;
    }

    bool UISystemImpl::IsHovered() const noexcept
    {
        if (!m_projectUI) return false;

        return m_projectUI->IsHovered();
    }

    void UISystemImpl::FrameBegin()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    #ifdef NC_EDITOR_ENABLED
    void UISystemImpl::Draw(float* dt, Registry* registry)
    {
        m_editor.Frame(dt, registry);
        if(m_projectUI)
        {
            m_projectUI->Draw();
        }

        ImGui::Render();
    }
    #else
    void UISystemImpl::Draw()
    {
        if(m_projectUI)
        {
            m_projectUI->Draw();
        }

        ImGui::Render();
    }
    #endif

} //end namespace nc::ui