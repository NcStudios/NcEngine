#include "UIImpl.h"
#include "UI.h"
#include "debug/Utils.h"
#include "graphics/Graphics.h"
#ifdef USE_VULKAN
#include "graphics/Graphics2.h"
#include "graphics/vulkan/Swapchain.h"
#include "imgui/imgui_impl_vulkan.h"
#endif
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
        #ifdef USE_VULKAN
    UIImpl::UIImpl(HWND hwnd, ::nc::graphics::Graphics2* graphics, ecs::MeshRendererSystem* meshRendererSystem, const ecs::Systems& systems)
    : m_editor{graphics, systems},
        m_projectUI{nullptr},
        m_graphics{graphics},
        m_meshRendererSystem{meshRendererSystem}
        #else
    UIImpl::UIImpl(HWND hwnd, ::nc::graphics::Graphics* graphics, const ecs::Systems& systems)
    : m_editor{graphics, systems},
        m_projectUI{nullptr}
        #endif
    #else
        #ifdef USE_VULKAN
    UIImpl::UIImpl(HWND hwnd, ::nc::graphics::Graphics2* graphics, ecs::MeshRendererSystem* meshRendererSystem)
    : m_projectUI{ nullptr },          
        m_graphics{graphics},
        m_meshRendererSystem{meshRendererSystem}
        #else
    UIImpl::UIImpl(HWND hwnd, ::nc::graphics::Graphics* graphics)
        : m_projectUI{ nullptr }
        #endif
    #endif
    {
        g_instance = this;
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hwnd);

        #ifdef USE_VULKAN
        auto& uiPassDefinition = m_graphics->GetSwapchainPtr()->GetPassDefinition();
        m_graphics->GetBasePtr()->InitializeImgui(uiPassDefinition);
        #else
        ImGui_ImplDX11_Init(graphics->m_device, graphics->m_context);
        #endif
    }

    UIImpl::~UIImpl() noexcept
    {
        #ifdef USE_VULKAN
        ImGui_ImplVulkan_Shutdown();
        #else
        ImGui_ImplDX11_Shutdown();
        #endif

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
        #ifdef USE_VULKAN
        ImGui_ImplVulkan_NewFrame();
        #else
        ImGui_ImplDX11_NewFrame();
        #endif
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    #ifdef NC_EDITOR_ENABLED
    void UIImpl::Frame(float* dt, ecs::EntityMap& activeEntities)
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
        #ifdef USE_VULKAN
        #else
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        #endif
    }
} //end namespace nc::ui