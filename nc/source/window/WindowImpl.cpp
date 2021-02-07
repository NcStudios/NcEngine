#include "WindowImpl.h"
#include "Window.h"
#include "config/Config.h"
#include "Core.h"
#include "debug/Utils.h"
#include "input/InputInternal.h"
#include "math/Math.h"

#include <algorithm>

namespace
{
    auto WND_CLASS_STYLE_FLAGS = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    auto WND_STYLE_FLAGS = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    nc::window::WindowImpl* g_instance = nullptr;
}

namespace nc::window
{
    /* Api Function Implementation */
    Vector2 GetDimensions()
    {
        IF_THROW(!g_instance, "window::GetDimensions - g_instance is not set");
        return g_instance->GetDimensions();
    }

    void RegisterOnResizeReceiver(IOnResizeReceiver* receiver)
    {
        IF_THROW(!g_instance, "window::RegisterOnResizeReceiver - g_instance is not set");
        g_instance->RegisterOnResizeReceiver(receiver);
    }

    void UnregisterOnResizeReceiver(IOnResizeReceiver* receiver)
    {
        IF_THROW(!g_instance, "window::UnregisterOnResizeReceiver - g_instance is not set");
        g_instance->UnregisterOnResizeReceiver(receiver);
    }

    /* WindowImpl */
    WindowImpl::WindowImpl(HINSTANCE instance)
        : m_onResizeReceivers{},
          m_hInstance{instance}
    {
        g_instance = this;
        GraphicsOnResizeCallback = nullptr;
        UIWndMessageCallback = nullptr;

        const auto& config = config::Get();

        m_wndClass = {};
        m_wndClass.style = WND_CLASS_STYLE_FLAGS;
        m_wndClass.lpfnWndProc = WindowImpl::WndProc;
        m_wndClass.hInstance = instance;
        m_wndClass.lpszClassName = TEXT(config.project.projectName.c_str());

        if(!RegisterClass(&m_wndClass))
        {
            throw std::runtime_error("Window::Constructor - failed to register wnd class");
        }

        auto nativeWidth = GetSystemMetrics(SM_CXFULLSCREEN);
        auto nativeHeight = GetSystemMetrics(SM_CYFULLSCREEN);

        if(config.graphics.useNativeResolution)
        {
            m_dimensions = Vector2{ (float)nativeWidth, (float)nativeHeight };
        }
        else
        {
            m_dimensions = Vector2{ (float)config.graphics.screenWidth, (float)config.graphics.screenHeight };
        }

        auto left = math::Clamp(((int)nativeWidth - (int)m_dimensions.x) / 2, 0, nativeWidth);
        auto top = math::Clamp(((int)nativeHeight - (int)m_dimensions.y) / 2, 0, nativeHeight);

        auto clientRect = RECT
        {
            (LONG)left,
            (LONG)top,
            (LONG)(left + m_dimensions.x),
            (LONG)(top + m_dimensions.y)
        };

        if(!AdjustWindowRect(&clientRect, WND_STYLE_FLAGS, FALSE))
        {
            throw std::runtime_error("Failed to adjust client rect to window rect");
        }

        if(clientRect.left < 0)
        {
            clientRect.right += (-1 * clientRect.left);
            clientRect.left = 0;
        }

        if(clientRect.top < 0)
        {
            clientRect.bottom += (-1 * clientRect.top);
            clientRect.top = 0;
        }

        m_hwnd = CreateWindowExA(0, (LPCSTR)m_wndClass.lpszClassName,
                                config.project.projectName.c_str(),
                                WND_STYLE_FLAGS,
                                clientRect.left, clientRect.top,
                                clientRect.right - clientRect.left,
                                clientRect.bottom - clientRect.top,
                                0, 0, instance, 0);
                                
        if(!m_hwnd)
        {
            throw std::runtime_error("Window constructor - CreateWindow failed");
        }
    }

    WindowImpl::~WindowImpl() noexcept
    {
        DestroyWindow(m_hwnd);
    }

    HWND WindowImpl::GetHWND() const noexcept
    {
        return m_hwnd;
    }

    HINSTANCE WindowImpl::GetHINSTANCE() const noexcept
    {
        return m_hInstance;
    }

    Vector2 WindowImpl::GetDimensions() const
    {
        return m_dimensions;
    }

    void WindowImpl::BindGraphicsOnResizeCallback(std::function<void(float,float,float,float)> callback)
    {
        GraphicsOnResizeCallback = callback;
    }

    void WindowImpl::BindUICallback(std::function<LRESULT(HWND,UINT,WPARAM,LPARAM)> callback)
    {
        UIWndMessageCallback = callback;
    }

    void WindowImpl::RegisterOnResizeReceiver(IOnResizeReceiver* receiver)
    {
        m_onResizeReceivers.push_back(receiver);
    }

    void WindowImpl::UnregisterOnResizeReceiver(IOnResizeReceiver* receiver)
    {
        auto pos = std::find(m_onResizeReceivers.begin(), m_onResizeReceivers.end(), receiver);
        if(pos == m_onResizeReceivers.end())
        {
            throw std::runtime_error("Attempt to unregister an unregistered IOnResizeReceiver");
        }

        *pos = m_onResizeReceivers.back();
        m_onResizeReceivers.pop_back();
    }

    void WindowImpl::OnResize(float width, float height)
    {
        if(!(GraphicsOnResizeCallback))
        {
            return;
        }

        m_dimensions = Vector2{width, height};
        const auto& config = config::Get();
        GraphicsOnResizeCallback(m_dimensions.x, m_dimensions.y, config.graphics.nearClip, config.graphics.farClip);
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
                g_instance->OnResize(LOWORD(lParam), HIWORD(lParam));
                break;
            }
            case WM_CLOSE:
            {
                core::Quit(false);
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

    void WindowImpl::ProcessSystemMessages()
    {
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
                    input::UpdateMousePosition(message.lParam);
                    break;
                }
                case WM_LBUTTONDOWN: //need to double check w/l params for mouse button events, don't think this is working
                case WM_LBUTTONUP:
                case WM_MBUTTONDOWN:
                case WM_MBUTTONUP:
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                case WM_SYSKEYDOWN:
                case WM_SYSKEYUP:
                case WM_KEYDOWN:
                case WM_KEYUP:
                {
                    input::AddToQueue(message.wParam, message.lParam);
                    break;
                }
            }
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }
} // end namespace nc::window