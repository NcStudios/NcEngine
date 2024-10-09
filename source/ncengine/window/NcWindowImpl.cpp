#include "NcWindowImpl.h"
#include "NcEngine.h"
#include "config/Config.h"
#include "graphics/GraphicsUtilities.h"
#include "input/InputInternal.h"
#include "window/Window.h"
#include "ui/ImGuiUtility.h"

#include "ncmath/Math.h"
#include "ncutility/NcError.h"
#include "utility/Log.h"

#include <algorithm>

namespace
{
    nc::window::NcWindowImpl* g_instance = nullptr;
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

    Vector2 ToNormalizedDeviceCoordinates(const Vector2& screenCoordinates)
    {
        NC_ASSERT(g_instance, "Window instance is not set");
        const auto& [screenX, screenY] = g_instance->GetScreenExtent();
        return Vector2{
            (2.0f * screenCoordinates.x) / screenX - 1.0f,
            (2.0f * screenCoordinates.y) / screenY - 1.0f
        };
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
                           bool isGraphicsEnabled,
                           Signal<>& quit) -> std::unique_ptr<NcWindow>
    {
        return std::make_unique<NcWindowImpl>(projectSettings, isGraphicsEnabled, quit);
    }

    /* NcWindowImpl */
    NcWindowImpl::NcWindowImpl(const config::ProjectSettings& projectSettings,
                               bool isGraphicsEnabled,
                               Signal<>& quit)
        : m_windowName{projectSettings.projectName},
          m_onResizeReceivers{},
          m_quit{&quit}
    {
        g_instance = this;
        m_window = nullptr;

        if (!glfwInit())
        {
            const char* error = nullptr;
            const auto code = glfwGetError(&error);
            throw NcError(fmt::format("Failed to initialize GLFW: {} ({}).", error, code));
        }

        /* We only want a headless window created by default here if graphics is disabled. 
           If graphics is enabled, the graphics module will set a new window (headless or otherwise)
           and we don't need to create a window needlessly. */
        if (!isGraphicsEnabled)
        {
            SetWindow(WindowInfo
            {
                .dimensions = Vector2{1, 1},
                .isGL = false,
                .isHeadless = true,
                .useNativeResolution = false,
                .launchInFullScreen = false,
                .isResizable = false
            });
        }
    }

    NcWindowImpl::~NcWindowImpl() noexcept
    {
        if (m_window)
        {
            glfwDestroyWindow(m_window);
        }
        glfwTerminate();
    }

    void NcWindowImpl::SetDimensions(int width, int height) noexcept
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

    void NcWindowImpl::RegisterOnResizeReceiver(IOnResizeReceiver* receiver)
    {
        m_onResizeReceivers.push_back(receiver);
    }

    void NcWindowImpl::UnregisterOnResizeReceiver(IOnResizeReceiver* receiver) noexcept
    {
        auto pos = std::find(m_onResizeReceivers.begin(), m_onResizeReceivers.end(), receiver);
        if(pos != m_onResizeReceivers.end())
        {
            *pos = m_onResizeReceivers.back();
            m_onResizeReceivers.pop_back();
        }
    }

    void NcWindowImpl::ProcessResizeEvent(GLFWwindow*, int width, int height)
    {
        g_instance->SetDimensions(width, height);
    }

    void NcWindowImpl::ProcessSetContentScaleEvent(GLFWwindow*, float x, float y)
    {
        g_instance->m_contentScale = Vector2{x, y};
    }

    void NcWindowImpl::SetWindow(WindowInfo windowInfo)
    {
        glfwWindowHint(GLFW_CLIENT_API, windowInfo.isGL ? GLFW_OPENGL_API : GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, windowInfo.isResizable);

        if (windowInfo.isHeadless)
        {
            glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

            if (m_window)
            {
                NC_LOG_TRACE("Tearing down the headless window.");
                glfwDestroyWindow(m_window);
            }
            m_window = glfwCreateWindow(1, 1, m_windowName.c_str(), nullptr, nullptr);
            NC_LOG_TRACE("Created a headless window.");
        }
        else
        {
            glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
            const auto* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            if (!videoMode)
            {
                throw NcError("Error getting the monitor's video mode information.");
            }

            auto nativeWidth = videoMode->width;
            auto nativeHeight = videoMode->height;

            if(windowInfo.useNativeResolution || windowInfo.launchInFullScreen)
            {
                m_dimensions = Vector2{ static_cast<float>(nativeWidth), static_cast<float>(nativeHeight) };
            }
            else
            {
                m_dimensions = windowInfo.dimensions;
            }

            m_screenExtent = graphics::AdjustDimensionsToAspectRatio(m_dimensions);
            auto width = Clamp((int)m_dimensions.x, 0, nativeWidth);
            auto height = Clamp((int)m_dimensions.y, 0, nativeHeight);
            auto monitor = windowInfo.launchInFullScreen ? glfwGetPrimaryMonitor() : nullptr;

            if (m_window)
            {
                NC_LOG_TRACE("Tearing down the window.");
                glfwDestroyWindow(m_window);
            }
            m_window = glfwCreateWindow(width, height, m_windowName.c_str(), monitor, nullptr);
            NC_LOG_TRACE("Created a window.");
        }

        if(!m_window)
        {
            throw NcError("SetWindow failed");
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

    void NcWindowImpl::ProcessSystemMessages()
    {
        glfwPollEvents();
    }

    void NcWindowImpl::ProcessKeyEvent(GLFWwindow*, int key, int, int action, int)
    {
        if (ui::IsCapturingKeyboard())
        {
            return;
        }

        nc::input::KeyCode_t keyCode = static_cast<nc::input::KeyCode_t>(key);
        nc::input::AddKeyToQueue(keyCode, action);
    }

    void NcWindowImpl::ProcessMouseCursorPosEvent(GLFWwindow*, double xPos, double yPos)
    {
        nc::input::UpdateMousePosition(static_cast<int>(xPos), static_cast<int>(yPos));
    }

    void NcWindowImpl::ProcessMouseButtonEvent(GLFWwindow*, int button, int action, int)
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

    void NcWindowImpl::ProcessMouseScrollEvent(GLFWwindow*, double, double yOffset)
    {
        if (ui::IsCapturingMouse())
        {
            return;
        }

        input::SetMouseWheel(static_cast<int>(yOffset));
    }

    void NcWindowImpl::ProcessWindowCloseEvent(GLFWwindow* window)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        g_instance->m_quit->Emit();
    }
} // end namespace nc::window
