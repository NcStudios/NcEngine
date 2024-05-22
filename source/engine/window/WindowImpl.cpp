#include "WindowImpl.h"
#include "NcEngine.h"
#include "config/Config.h"
#include "graphics/GraphicsUtilities.h"
#include "input/InputInternal.h"
#include "window/Window.h"
#include "ui/ImGuiUtility.h"

#include "ncmath/Math.h"
#include "ncutility/NcError.h"

#include <algorithm>

namespace
{
    nc::window::WindowImpl* g_instance = nullptr;
}

namespace nc::window
{
    /* Api Function Implementation */
    Vector2 GetDimensions()
    {
        NC_ASSERT(g_instance, "Window instance is not set");
        return g_instance->GetDimensions();
    }

    Vector2 GetScreenExtent()
    {
        NC_ASSERT(g_instance, "Window instance is not set");
        return g_instance->GetScreenExtent();
    }

    Vector2 GetContentScale()
    {
        NC_ASSERT(g_instance, "Window instance is not set");
        return g_instance->GetContentScale();
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

    auto BuildWindowModule(const config::ProjectSettings& projectSettings,
                           const config::GraphicsSettings& graphicsSettings,
                           Signal<>& quit) -> std::unique_ptr<NcWindow>
    {
        return std::make_unique<WindowImpl>(projectSettings, graphicsSettings, quit);
    }

    /* WindowImpl */
    WindowImpl::WindowImpl(const config::ProjectSettings& projectSettings,
                           const config::GraphicsSettings& graphicsSettings,
                           Signal<>& quit)
        : m_onResizeReceivers{},
        //   m_dimensions{},
          m_quit{&quit}
    {
        g_instance = this;

        if (!glfwInit())
        {
            const char* error = nullptr;
            const auto code = glfwGetError(&error);
            throw NcError(fmt::format("Failed to initialize GLFW: {} ({}).", error, code));
        }

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

        m_screenExtent = graphics::AdjustDimensionsToAspectRatio(m_dimensions);
        auto width = Clamp((int)m_dimensions.x, 0, nativeWidth);
        auto height = Clamp((int)m_dimensions.y, 0, nativeHeight);
        auto monitor = graphicsSettings.launchInFullscreen ? glfwGetPrimaryMonitor() : nullptr;
        m_window = glfwCreateWindow(width, height, projectSettings.projectName.c_str(), monitor, nullptr);

        if(!m_window)
        {
            throw NcError("CreateWindow failed");
        }

        glfwGetWindowContentScale(m_window, &m_contentScale.x, &m_contentScale.y);

        glfwSetKeyCallback(m_window, &ProcessKeyEvent);
        glfwSetCursorPosCallback(m_window, &ProcessMouseCursorPosEvent);
        glfwSetMouseButtonCallback(m_window, &ProcessMouseButtonEvent);
        glfwSetScrollCallback(m_window, &ProcessMouseScrollEvent);
        glfwSetWindowSizeCallback(m_window, &ProcessResizeEvent);
        glfwSetWindowContentScaleCallback(m_window, &ProcessSetContentScaleEvent);
        glfwSetWindowCloseCallback(m_window, &ProcessWindowCloseEvent);
    }

    WindowImpl::~WindowImpl() noexcept
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }


    void WindowImpl::SetDimensions(int width, int height) noexcept
    {
        m_dimensions = Vector2{static_cast<float>(width), static_cast<float>(height)};
        m_screenExtent = graphics::AdjustDimensionsToAspectRatio(m_dimensions);
        glfwSetWindowSize(m_window, width, height);
        const auto minimized = static_cast<bool>(glfwGetWindowAttrib(m_window, GLFW_ICONIFIED));
        m_onResize.Emit(m_dimensions, minimized);

        for(auto receiver : m_onResizeReceivers)
        {
            receiver->OnResize(m_dimensions);
        }
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

    void WindowImpl::ProcessResizeEvent(GLFWwindow*, int width, int height)
    {
        g_instance->SetDimensions(width, height);
    }

    void WindowImpl::ProcessSetContentScaleEvent(GLFWwindow*, float x, float y)
    {
        g_instance->m_contentScale = Vector2{x, y};
    }

    void WindowImpl::ProcessSystemMessages()
    {
        glfwPollEvents();
    }

    void WindowImpl::ProcessKeyEvent(GLFWwindow*, int key, int, int action, int)
    {
        if (ui::IsCapturingKeyboard())
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

        if (ui::IsCapturingMouse() || button >= static_cast<int>(mouseLUT.size()))
        {
            return;
        }

        const auto mouseButton = mouseLUT.at(button);
        AddKeyToQueue(mouseButton, action);
    }

    void WindowImpl::ProcessMouseScrollEvent(GLFWwindow*, double, double yOffset)
    {
        if (ui::IsCapturingMouse())
        {
            return;
        }

        input::SetMouseWheel(static_cast<int>(yOffset));
    }

    void WindowImpl::ProcessWindowCloseEvent(GLFWwindow* window)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        g_instance->m_quit->Emit();
    }
} // end namespace nc::window
