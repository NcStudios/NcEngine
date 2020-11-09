#include "Window.h"
#include "engine/Engine.h"
#include "NcConfig.h"
#include "graphics/Graphics.h"
#include "NcDebug.h"
#include "input/Input.h"
#include "ui/UISystem.h"
#include "math/Math.h"

namespace
{
    auto WND_CLASS_STYLE_FLAGS = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    auto WND_STYLE_FLAGS = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
}

namespace nc
{

Window* Window::Instance = nullptr;

Window::Window(HINSTANCE instance, engine::Engine* engine, const config::Config& config)
{
    Window::Instance = this;
    m_engine = engine;
    m_graphics = nullptr;

    m_wndClass = {};
    m_wndClass.style = WND_CLASS_STYLE_FLAGS;
    m_wndClass.lpfnWndProc = Window::WndProc;
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
        m_dimensions = { (float)nativeWidth, (float)nativeHeight };
    }
    else
    {
        m_dimensions = { (float)config.graphics.screenWidth, (float)config.graphics.screenHeight };
    }

    auto left = math::Clamp(((int)nativeWidth - (int)m_dimensions.X()) / 2, 0, nativeWidth);
    auto top = math::Clamp(((int)nativeHeight - (int)m_dimensions.Y()) / 2, 0, nativeHeight);

    auto clientRect = RECT
    {
        (LONG)left,
        (LONG)top,
        (LONG)(left + m_dimensions.X()),
        (LONG)(top + m_dimensions.Y())
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

Window::~Window() noexcept
{
    DestroyWindow(m_hwnd);
}

HWND Window::GetHWND() const noexcept
{
    return m_hwnd;
}

Vector2 Window::GetWindowDimensions() const
{
    return m_dimensions;
}

void Window::BindUISystem(ui::UISystem* ui)
{
    m_ui = ui;
}

void Window::BindGraphics(graphics::Graphics* graphics)
{
    m_graphics = graphics;
}

void Window::OnResize(float width, float height)
{
    if(!(m_graphics))
    {
        return;
    }

    m_dimensions = {width, height};
    const auto& config = config::NcGetConfigReference();
    m_graphics->OnResize(m_dimensions.X(), m_dimensions.Y(), config.graphics.nearClip, config.graphics.farClip);
}

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(Window::Instance->m_ui->WndProc(hwnd, message, wParam, lParam))
    {
        return true;
    }

    switch(message)
	{
        case WM_SIZE:
        {
            Window::Instance->OnResize(LOWORD(lParam), HIWORD(lParam));
            break;
        }
        case WM_CLOSE:
        {
            Window::Instance->m_engine->Shutdown();
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

void Window::ProcessSystemMessages()
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
} // end namespace nc