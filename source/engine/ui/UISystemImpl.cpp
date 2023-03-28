#include "UISystemImpl.h"

#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_vulkan.h"
#include "imgui/imgui_impl_glfw.h"

namespace nc::ui
{
    #ifdef NC_EDITOR_ENABLED
    UISystemImpl::UISystemImpl(GLFWwindow* window)
    : m_editor{},
      m_projectUI{nullptr}
    #else
    UISystemImpl::UISystemImpl(GLFWwindow* window)
    : m_projectUI{nullptr}
    #endif
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForVulkan(window, true);
    }

    UISystemImpl::~UISystemImpl() noexcept
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui::DestroyContext();
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
        ImGui_ImplGlfw_NewFrame();
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