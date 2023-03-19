#include "WindowImpl.h"
#include "NcEngine.h"
#include "config/Config.h"
#include "input/InputInternal.h"
#include "ncmath/Math.h"
#include "ncutility/NcError.h"
#include "window/Window.h"

#include <algorithm>
#include <iostream>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
namespace
{
    nc::window::WindowImpl* g_instance = nullptr;

    void PrintGlfwErrorCallback(int error, const char* description)
    {
        std::cerr << "GLFW error: " << error << " description: "<< description << std::endl;
    }
}

namespace nc::window
{
    /* Api Function Implementation */
    Vector2 GetDimensions()
    {
        NC_ASSERT(g_instance, "window::GetDimensions - g_instance is not set");
        return g_instance->GetDimensions();
    }

    void RegisterOnResizeReceiver(IOnResizeReceiver* receiver)
    {
        NC_ASSERT(g_instance, "window::RegisterOnResizeReceiver - g_instance is not set");
        g_instance->RegisterOnResizeReceiver(receiver);
    }

    void UnregisterOnResizeReceiver(IOnResizeReceiver* receiver) noexcept
    {
        g_instance->UnregisterOnResizeReceiver(receiver);
    }

    /* WindowImpl */
    WindowImpl::WindowImpl(std::function<void()> onQuit)
        : m_onResizeReceivers{},
          m_dimensions{},
          GraphicsOnResizeCallback{nullptr},
          UIWndMessageCallback{nullptr},
          EngineDisableRunningCallback{std::move(onQuit)}
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        g_instance = this;

        const auto& projectSettings = config::GetProjectSettings();
        const auto& graphicsSettings = config::GetGraphicsSettings();

        const auto* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());

        auto nativeWidth = videoMode->width;
        auto nativeHeight = videoMode->height;

        if(graphicsSettings.useNativeResolution)
        {
            m_dimensions = Vector2{ static_cast<float>(nativeWidth), static_cast<float>(nativeHeight) };
        }
        else
        {
            m_dimensions = Vector2{ static_cast<float>(graphicsSettings.screenWidth), static_cast<float>(graphicsSettings.screenHeight) };
        }

        auto width = Clamp((int)m_dimensions.x, 0, nativeWidth);
        auto height = Clamp((int)m_dimensions.y, 0, nativeHeight);

        m_window = glfwCreateWindow(width, height, projectSettings.projectName.c_str(), nullptr, nullptr);
        nc::input::SetWindow(m_window);

        if(!m_window)
        {
            throw NcError("CreateWindow failed");
        }

        glfwSetKeyCallback(m_window, &ProcessKeyEvent);
        glfwSetCursorPosCallback(m_window, &ProcessMouseCursorPosEvent);
        glfwSetMouseButtonCallback(m_window, &ProcessMouseButtonEvent);
        glfwSetScrollCallback(m_window, &ProcessMouseScrollEvent);
        glfwSetErrorCallback(&PrintGlfwErrorCallback);
    }

    WindowImpl::~WindowImpl() noexcept
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    auto WindowImpl::GetWindow() -> GLFWwindow*
    {
        return m_window;
    }

    Vector2 WindowImpl::GetDimensions() const noexcept
    {
        return m_dimensions;
    }

    void WindowImpl::BindGraphicsOnResizeCallback(std::function<void(float,float,float,float,WPARAM)> callback) noexcept
    {
        GraphicsOnResizeCallback = std::move(callback);
    }

    void WindowImpl::BindUICallback(std::function<LRESULT(HWND,UINT,WPARAM,LPARAM)> callback) noexcept
    {
        //UIWndMessageCallback = std::move(callback);
    }

    void WindowImpl::RegisterOnResizeReceiver(IOnResizeReceiver* receiver)
    {
        m_onResizeReceivers.push_back(receiver);
    }

    void WindowImpl::UnregisterOnResizeReceiver(IOnResizeReceiver* receiver) noexcept
    {
        auto pos = std::find(m_onResizeReceivers.begin(), m_onResizeReceivers.end(), receiver);
        if(pos != m_onResizeReceivers.end())
        {
            *pos = m_onResizeReceivers.back();
            m_onResizeReceivers.pop_back();
        }
    }

    void WindowImpl::OnResize(float width, float height, WPARAM windowArg)
    {
        if(!GraphicsOnResizeCallback)
        {
            return;
        }

        m_dimensions = Vector2{width, height};
        glfwSetWindowSize(m_window, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
        const auto& graphicsSettings = config::GetGraphicsSettings();
        GraphicsOnResizeCallback(m_dimensions.x, m_dimensions.y, graphicsSettings.nearClip, graphicsSettings.farClip, windowArg);
        for(auto receiver : m_onResizeReceivers)
        {
            receiver->OnResize(m_dimensions);
        }
    }

    void WindowImpl::PollEvents()
    {
        if (glfwWindowShouldClose(m_window))
        {
            g_instance->EngineDisableRunningCallback();
        }

        glfwPollEvents();
    }

    void WindowImpl::ProcessKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        nc::input::ButtonCode_t keyCode = static_cast<nc::input::ButtonCode_t>(key);
        if (mods == GLFW_MOD_SHIFT) keyCode = static_cast<nc::input::ButtonCode_t>(nc::input::KeyCode::Shift);
        else if (mods == GLFW_MOD_ALT) keyCode = static_cast<nc::input::ButtonCode_t>(nc::input::KeyCode::Alt);
        else if (mods == GLFW_MOD_CONTROL) keyCode = static_cast<nc::input::ButtonCode_t>(nc::input::KeyCode::Ctrl);
        
        nc::input::AddKeyToQueue(keyCode, action);
    }

    void WindowImpl::ProcessMouseCursorPosEvent(GLFWwindow* window, double xPos, double yPos)
    {
        int windowFrameHeight;
        int windowFrameWidth;

        ImGuiIO& io = ImGui::GetIO();
        io.MousePos = ImVec2(static_cast<float>(xPos), static_cast<float>(yPos));
        glfwGetWindowSize(window, &windowFrameWidth, &windowFrameHeight);
        nc::input::UpdateMousePosition(static_cast<int>(xPos), static_cast<int>(yPos)); // windowFrameHeight - static_cast<int>(yPos) - 1);
    }

    void WindowImpl::ProcessMouseButtonEvent(GLFWwindow* window, int button, int action, int mods)
    {
        using namespace nc::input;
        ButtonCode_t mouseButton;
        ImGuiIO& io = ImGui::GetIO();

        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            mouseButton = (ButtonCode_t)MouseCode::LeftButton;
        }
        else if (button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            mouseButton = (ButtonCode_t)MouseCode::RightButton;
        }
        else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
        {
            mouseButton = (ButtonCode_t)MouseCode::MiddleButton;
        }
        else
        {
            throw NcError(std::string{"Invalid mouse button pressed."});
        }

        if (action == GLFW_PRESS)
        {
            AddMouseButtonDownToQueue((ButtonCode_t)mouseButton, action);
            io.MouseClicked[(ButtonCode_t)mouseButton] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            AddMouseButtonUpToQueue((ButtonCode_t)mouseButton, action);
            io.MouseReleased[(ButtonCode_t)mouseButton] = true;
        }
        else
        {
            throw NcError(std::string{"Invalid mouse button action selected."});
        }
    }

    void WindowImpl::ProcessMouseScrollEvent(GLFWwindow* window, double xOffset, double yOffset)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.MouseWheel = static_cast<float>(yOffset);
        input::SetMouseWheel(static_cast<int>(xOffset), static_cast<int>(yOffset));
    }

} // end namespace nc::window