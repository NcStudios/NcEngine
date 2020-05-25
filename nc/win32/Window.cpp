#include "Window.h"
#include "NCE.h"
#include "ProjectSettings.h"
#include <iostream>

#ifdef NC_DEBUG
#include "EditorManager.h"
#endif

namespace nc {

Window* Window::Instance = nullptr;

Window::Window(HINSTANCE instance) noexcept
{
    Window::Instance = this; //is it worth doing this the other way?
    
    m_wndClass = {};
    m_wndClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    m_wndClass.lpfnWndProc = Window::WndProc;
    m_wndClass.hInstance = instance;
    m_wndClass.lpszClassName = TEXT("PROJECT NAME"); //TEXT(ProjectSettings::projectName.c_str());

    if(!RegisterClass(&m_wndClass)) std::cerr << "Failed to retrieve window rect" << std::endl; //should throw/terminate

    m_hwnd = CreateWindowExA(0, (LPCSTR)m_wndClass.lpszClassName,
                             ProjectSettings::projectName.c_str(),
                             WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                             0, 0, ProjectSettings::displaySettings.screenWidth, ProjectSettings::displaySettings.screenHeight,
                             0, 0, instance, 0);
                             
    if(!m_hwnd) std::cerr<< "HWND not found" << std::endl; //should throw/terminate

    m_deviceContext = GetDC(m_hwnd);
    m_windowDimensions = GetWindowDimensions();
}

Window::~Window() noexcept
{
    DestroyWindow(m_hwnd);
}

HWND Window::GetHWND() const noexcept
{
    return m_hwnd;
}

#ifdef NC_DEBUG
void Window::BindEditorManager(utils::editor::EditorManager* editorManager)
{
    m_editorManager = editorManager;
}
#endif

void Window::OnWindowResize()
{
    m_windowDimensions = GetWindowDimensions();
    //PatBlt(m_deviceContext, 0, 0, m_windowDimensions.first, m_windowDimensions.second, BLACKNESS);
}

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    #ifdef NC_DEBUG
    if(Window::Instance->m_editorManager->WndProc(hwnd, message, wParam, lParam))
    {
        return true;
    }
    #endif

    switch(message)
	{
        case WM_SIZE:        { Window::Instance->OnWindowResize();      } break;
        case WM_CLOSE:       { NCE::Exit();                             } break;
        case WM_DESTROY:     { NCE::Exit();                             } break;
        case WM_MOUSEWHEEL:  { input::SetMouseWheel(wParam, lParam);    } break;
        // case WM_LBUTTONDOWN: { input::SetLeftButton(wParam, lParam);    } break;
        // case WM_MBUTTONDOWN: { input::SetMiddleButton(wParam, lParam);  } break;
        // case WM_RBUTTONDOWN: { input::SetRightButton(wParam, lParam);   } break;

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
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
            case WM_QUIT:      { NCE::Exit();                                       } break;
            case WM_MOUSEMOVE: { input::UpdateMousePosition(message.lParam);        } break;
            case WM_LBUTTONDOWN: //need to double check w/l params for mouse button events, don't think this is working
            case WM_LBUTTONUP:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
            case WM_SYSKEYDOWN:// |
            case WM_SYSKEYUP:  // |
            case WM_KEYDOWN:   // v
            case WM_KEYUP:     { input::AddToQueue(message.wParam, message.lParam); } break;
        }
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
}

std::pair<int, int> Window::GetWindowDimensions() const noexcept
{
    RECT windowRect;
    std::pair<int, int> dimensions;
    if(GetWindowRect(m_hwnd, &windowRect))
    {
        dimensions.first = windowRect.right - windowRect.left;
        dimensions.second = windowRect.bottom - windowRect.top;
    }
    else
    {
        std::cerr << "Failed to retrieve window rect" << std::endl;
    }
    return dimensions;
}


} // end namespace nc