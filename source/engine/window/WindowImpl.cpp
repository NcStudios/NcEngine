#include "WindowImpl.h"
#include "NcEngine.h"
#include "config/Config.h"
#include "input/InputInternal.h"
#include "ncmath/Math.h"
#include "ncutility/NcError.h"
#include "window/Window.h"

#include <algorithm>

namespace
{
    constexpr auto WndClassStyleFlags = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    constexpr auto WndStyleFlags = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
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
          m_hwnd{nullptr},
          m_wndClass{},
          m_hInstance{},
          m_dimensions{},
          GraphicsOnResizeCallback{nullptr},
          UIWndMessageCallback{nullptr},
          EngineDisableRunningCallback{std::move(onQuit)}
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

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

        if(!m_window)
        {
            throw NcError("CreateWindow failed");
        }
    }

    WindowImpl::~WindowImpl() noexcept
    {
        DestroyWindow(m_hwnd);
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    auto WindowImpl::GetGlfwWindow() -> GLFWwindow*
    {
        return m_window;
    }

    HWND WindowImpl::GetHWND() const noexcept
    {
        return m_hwnd;
    }

    HINSTANCE WindowImpl::GetHINSTANCE() const noexcept
    {
        return m_hInstance;
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
        UIWndMessageCallback = std::move(callback);
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
        if(!(GraphicsOnResizeCallback))
        {
            return;
        }

        m_dimensions = Vector2{width, height};
        const auto& graphicsSettings = config::GetGraphicsSettings();
        GraphicsOnResizeCallback(m_dimensions.x, m_dimensions.y, graphicsSettings.nearClip, graphicsSettings.farClip, windowArg);
        for(auto receiver : m_onResizeReceivers)
        {
            receiver->OnResize(m_dimensions);
        }
    }

    LRESULT CALLBACK WindowImpl::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if(g_instance->UIWndMessageCallback &&
           g_instance->UIWndMessageCallback(hwnd, message, wParam, lParam))
        {
            return true;
        }

        switch(message)
        {
            case WM_SIZE:
            {
                g_instance->OnResize(LOWORD(lParam), HIWORD(lParam), wParam);
                break;
            }
            case WM_CLOSE:
            {
                g_instance->EngineDisableRunningCallback();
                break;
            }
            case WM_DESTROY:
            {
                PostQuitMessage(0);
                break;
            }
            case WM_QUIT:
            {
                break;
            }
            case WM_MOUSEWHEEL:
            {
                input::SetMouseWheel(wParam, lParam);
                break;
            }
            default:
            {
                return DefWindowProc(hwnd, message, wParam, lParam);
            }
        }
        return 0;
    }

    void WindowImpl::PollEvents()
    {
        if (glfwWindowShouldClose(m_window))
        {
            g_instance->EngineDisableRunningCallback();
        }

        glfwPollEvents();
    }

    void WindowImpl::ProcessSystemMessages()
    {
        using namespace nc::input;

        MSG message;
        while(PeekMessage(&message, 0, 0, 0, PM_REMOVE))
        {
            switch (message.message)
            {
                case WM_QUIT:
                {
                    break;
                }
                case WM_MOUSEMOVE:
                {
                    UpdateMousePosition(message.lParam);
                    break;
                }
                case WM_LBUTTONDOWN:
                {
                    AddMouseButtonDownToQueue((KeyCode_t)KeyCode::LeftButton, message.lParam);
                    break;
                }
                case WM_LBUTTONUP:
                {
                    AddMouseButtonUpToQueue((KeyCode_t)KeyCode::LeftButton, message.lParam);
                    break;
                }
                case WM_MBUTTONDOWN:
                {
                    AddMouseButtonDownToQueue((KeyCode_t)KeyCode::MiddleButton, message.lParam);
                    break;
                }
                case WM_MBUTTONUP:
                {
                    AddMouseButtonUpToQueue((KeyCode_t)KeyCode::MiddleButton, message.lParam);
                    break;
                }
                case WM_RBUTTONDOWN:
                {
                    AddMouseButtonDownToQueue((KeyCode_t)KeyCode::RightButton, message.lParam);
                    break;
                }
                case WM_RBUTTONUP:
                {
                    AddMouseButtonUpToQueue((KeyCode_t)KeyCode::RightButton, message.lParam);
                    break;
                }
                case WM_SYSKEYDOWN:
                case WM_SYSKEYUP:
                case WM_KEYDOWN:
                case WM_KEYUP:
                {
                    AddKeyToQueue(static_cast<KeyCode_t>(message.wParam), message.lParam);
                    break;
                }
            }
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }
} // end namespace nc::window