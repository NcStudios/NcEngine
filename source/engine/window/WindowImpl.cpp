#include "WindowImpl.h"
#include "NcEngine.h"
#include "config/Config.h"
#include "input/InputInternal.h"
#include "ncmath/Math.h"
#include "ncutility/NcError.h"
#include "window/Window.h"

#include <algorithm>



#include "imgui/imgui.h"

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
    WindowImpl::WindowImpl(const config::ProjectSettings& projectSettings,
                           const config::GraphicsSettings& graphicsSettings,
                           std::function<void()> onQuit)
        : m_onResizeReceivers{},
          m_dimensions{},
          GraphicsOnResizeCallback{nullptr},
          EngineDisableRunningCallback{std::move(onQuit)}
    {
        g_instance = this;

        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        const auto* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        if (!videoMode)
        {
            throw NcError("Error getting the monitor's video mode information.");
        }

        auto nativeWidth = videoMode->width;
        auto nativeHeight = videoMode->height;

        if(graphicsSettings.useNativeResolution || graphicsSettings.launchInFullscreen)
        {
            m_dimensions = Vector2{ static_cast<float>(nativeWidth), static_cast<float>(nativeHeight) };
        }
        else
        {
            m_dimensions = Vector2{ static_cast<float>(graphicsSettings.screenWidth), static_cast<float>(graphicsSettings.screenHeight) };
        }

        auto width = Clamp((int)m_dimensions.x, 0, nativeWidth);
        auto height = Clamp((int)m_dimensions.y, 0, nativeHeight);
        auto monitor = graphicsSettings.launchInFullscreen ? glfwGetPrimaryMonitor() : nullptr;
        m_window = glfwCreateWindow(width, height, projectSettings.projectName.c_str(), monitor, nullptr);

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

    void WindowImpl::BindGraphicsOnResizeCallback(std::function<void(float,float,bool)> callback) noexcept
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
        if(!(GraphicsOnResizeCallback))
        {
            return;
        }

        int minimized = glfwGetWindowAttrib(window, GLFW_ICONIFIED);
        GraphicsOnResizeCallback(static_cast<float>(width), static_cast<float>(height), minimized);

        for(auto receiver : m_onResizeReceivers)
        {
            receiver->OnResize(m_dimensions);
        }
    }

    void WindowImpl::ProcessResizeEvent(GLFWwindow* window, int width, int height)
    {
        g_instance->SetDimensions(width, height);
        glfwSetWindowSize(window, width, height);
        g_instance->InvokeResizeReceivers(window, width, height);
    }

    void WindowImpl::ProcessSystemMessages()
    {
        if (glfwWindowShouldClose(m_window))
        {
            EngineDisableRunningCallback();
        }

        glfwPollEvents();
    }

    void WindowImpl::ProcessKeyEvent(GLFWwindow*, int key, int, int action, int)
    {
        auto& io = ImGui::GetIO();
        if (io.WantCaptureKeyboard)
        {
            return;
        }

        nc::input::KeyCode_t keyCode = static_cast<nc::input::KeyCode_t>(key);
        nc::input::AddKeyToQueue(keyCode, action);
    }

    void WindowImpl::ProcessMouseCursorPosEvent(GLFWwindow*, double xPos, double yPos)
    {
        nc::input::UpdateMousePosition(static_cast<int>(xPos), static_cast<int>(yPos));
    }

    void WindowImpl::ProcessMouseButtonEvent(GLFWwindow*, int button, int action, int)
    {
        if (ImGui::GetIO().WantCaptureMouse)
            return;

        using namespace nc::input;

        static constexpr auto mouseLUT = std::array<KeyCode_t, 8>
        {
            (KeyCode_t)KeyCode::LeftButton,
            (KeyCode_t)KeyCode::RightButton,
            (KeyCode_t)KeyCode::MiddleButton,
            (KeyCode_t)KeyCode::MouseButton4,
            (KeyCode_t)KeyCode::MouseButton5,
            (KeyCode_t)KeyCode::MouseButton6,
            (KeyCode_t)KeyCode::MouseButton7,
            (KeyCode_t)KeyCode::MouseButton8
        };
        
        if (button >= static_cast<int>(mouseLUT.size()))
        {
            return;
        }

        const auto mouseButton = mouseLUT.at(button);
        AddKeyToQueue(mouseButton, action);
    }

    void WindowImpl::ProcessMouseScrollEvent(GLFWwindow*, double, double yOffset)
    {
        if (ImGui::GetIO().WantCaptureMouse)
            return;

        input::SetMouseWheel(static_cast<int>(yOffset));
    }

    void WindowImpl::ProcessWindowCloseEvent(GLFWwindow* window)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        g_instance->EngineDisableRunningCallback();
    }
} // end namespace nc::window
