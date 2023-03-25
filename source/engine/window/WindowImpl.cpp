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
        glfwSetWindowSizeCallback(m_window, &ProcessResizeEvent);
        glfwSetWindowCloseCallback(m_window, &ProcessWindowCloseEvent);
    }

    WindowImpl::~WindowImpl() noexcept
    {
        glfwSetWindowShouldClose(m_window, GLFW_TRUE);
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

    void WindowImpl::SetDimensions(int width, int height) noexcept
    {
        m_dimensions = Vector2{static_cast<float>(width), static_cast<float>(height)};
    }

    void WindowImpl::BindGraphicsOnResizeCallback(std::function<void(float,float,float,float,bool)> callback) noexcept
    {
        GraphicsOnResizeCallback = std::move(callback);
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

    void WindowImpl::InvokeResizeReceivers(GLFWwindow* window, int width, int height)
    {
        const auto& graphicsSettings = config::GetGraphicsSettings();
        int minimized = glfwGetWindowAttrib(window, GLFW_ICONIFIED);
        g_instance->GraphicsOnResizeCallback(static_cast<float>(width), static_cast<float>(height), graphicsSettings.nearClip, graphicsSettings.farClip, minimized);
        for(auto receiver : m_onResizeReceivers)
        {
            receiver->OnResize(m_dimensions);
        }
    }

    void WindowImpl::ProcessResizeEvent(GLFWwindow* window, int width, int height)
    {
        if(!g_instance->GraphicsOnResizeCallback)
        {
            return;
        }

        g_instance->SetDimensions(width, height);
        glfwSetWindowSize(window, width, height);
        g_instance->InvokeResizeReceivers(window, width, height);
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
        nc::input::KeyCode_t keyCode = static_cast<nc::input::KeyCode_t>(key);
        if (mods == GLFW_MOD_SHIFT) keyCode = static_cast<nc::input::KeyCode_t>(nc::input::KeyCode::Shift);
        else if (mods == GLFW_MOD_ALT) keyCode = static_cast<nc::input::KeyCode_t>(nc::input::KeyCode::Alt);
        else if (mods == GLFW_MOD_CONTROL) keyCode = static_cast<nc::input::KeyCode_t>(nc::input::KeyCode::Ctrl);
        
        nc::input::AddKeyToQueue(keyCode, action);
    }

    void WindowImpl::ProcessMouseCursorPosEvent(GLFWwindow* window, double xPos, double yPos)
    {
        nc::input::UpdateMousePosition(static_cast<int>(xPos), static_cast<int>(yPos));
    }

    void WindowImpl::ProcessMouseButtonEvent(GLFWwindow* window, int button, int action, int mods)
    {
        using namespace nc::input;

        // TODO: could have more than 3 mouse buttons
        static constexpr auto mouseLUT = std::array<KeyCode_t, 3>
        {
            (KeyCode_t)KeyCode::LeftButton,
            (KeyCode_t)KeyCode::RightButton,
            (KeyCode_t)KeyCode::MiddleButton
        };

        const auto mouseButton = mouseLUT.at(button);
        AddKeyToQueue(mouseButton, action);
    }

    void WindowImpl::ProcessMouseScrollEvent(GLFWwindow* window, double xOffset, double yOffset)
    {
        input::SetMouseWheel(static_cast<int>(xOffset), static_cast<int>(yOffset));
    }

    void WindowImpl::ProcessWindowCloseEvent(GLFWwindow* window)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        g_instance->EngineDisableRunningCallback();
    }
} // end namespace nc::window