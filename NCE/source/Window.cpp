#include "../include/Window.h"
#include "../include/NCE.h"
#include "../include/Input.h"
#include "../include/ProjectSettings.h"

#include "../../graphics/primitive/Cube.h"

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

    if(!RegisterClass(&m_wndClass)) OutputDebugStr(TEXT("Failed to retrieve window rect")); //should throw/terminate

    m_hwnd = CreateWindowExA(0, (LPCSTR)m_wndClass.lpszClassName,
                             ProjectSettings::projectName.c_str(),
                             WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                             0, 0, ProjectSettings::displaySettings.screenWidth * 2,
                             ProjectSettings::displaySettings.screenHeight * 2,
                             0, 0, instance, 0);
                             
    if(!m_hwnd) OutputDebugStr(TEXT("HWND not found")); //should throw/terminate

    m_deviceContext = GetDC(m_hwnd);
    m_windowDimensions = GetWindowDimensions();

    m_graphics = std::make_unique<graphics::internal::Graphics>(m_hwnd, m_windowDimensions.width, m_windowDimensions.height);


    std::mt19937 rng( std::random_device{}() );
	std::uniform_real_distribution<float> adist( 0.0f,3.1415f * 2.0f );
	std::uniform_real_distribution<float> ddist( 0.0f,3.1415f * 2.0f );
	std::uniform_real_distribution<float> odist( 0.0f,3.1415f * 0.3f );
	std::uniform_real_distribution<float> rdist( 6.0f,20.0f );

    //make cubes
    for(auto i = 0; i < 20; ++i)
    {
        cubes.push_back
        (
            std::make_unique<nc::graphics::primitive::Cube>
            (
                *m_graphics.get(), rng, adist, ddist, odist, rdist
            )
        );
    }

    m_graphics->SetProjection
    (
        DirectX::XMMatrixPerspectiveLH
        (
            1.0f, 
            (float)ProjectSettings::displaySettings.screenHeight / (float)ProjectSettings::displaySettings.screenWidth,
            0.5f,
            40.0f
        )
    );

}

Window::~Window() noexcept
{
    DestroyWindow(m_hwnd);
}

void Window::OnWindowResize()
{
    m_windowDimensions = GetWindowDimensions();
    PatBlt(m_deviceContext, 0, 0, m_windowDimensions.width, m_windowDimensions.height, BLACKNESS);
}

graphics::internal::Graphics& Window::GetGraphics()
{
    return *m_graphics;
}

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
	{
        case WM_CREATE:          
            break;
        case WM_SIZE:
        {
            Window::Instance->OnWindowResize();
        }
        break;

        case WM_CLOSE:
        {
            NCE::Exit();
        }
        break;
        
        case WM_ACTIVATEAPP:
            break;

        case WM_DESTROY:
        {
            NCE::Exit();
        }
        break;

        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            BeginPaint(hwnd, &paint);
            EndPaint(hwnd, &paint);
        }
        break;

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}


void Window::CopyBufferToScreen(void *buffer, BITMAPINFO &bufferInfo, int width, int height)
{
    // HDC deviceContext = GetDC(Window::Instance->m_hwnd);
    // int xOffset = (Window::Instance->m_windowDimensions.width - width) / 2;
    // int yOffset = (Window::Instance->m_windowDimensions.height - height) / 2;

    // StretchDIBits(deviceContext,                   //destination (screen)
    //               xOffset, yOffset, width, height, //destination x, y, w, h
    //               0, 0, width, height,             //source x, y, w, h
    //               buffer, &bufferInfo,             //source buffer, buffer formatting   
    //               DIB_RGB_COLORS, SRCCOPY);        //RGB data flag, copy bits flag 

    //Window::Instance->m_graphics->EndFrame();


    auto dt = nc::time::Time::FrameDeltaTime;

    auto wnd = Window::Instance;

    wnd->m_graphics->ClearBuffer(0.0f, 0.0f, 0.0f);

    for(auto& c : wnd->cubes)
    {
        c->Update(dt);
        c->Draw(*(wnd->m_graphics.get()));
    }

    Window::Instance->m_graphics->EndFrame();
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
                NCE::Exit();
            } 
            break;

            case WM_MOUSEMOVE:
            {
                input::UpdateMousePosition(message.lParam);
            }
            break;

            case WM_SYSKEYDOWN: //may want to process syskeys separately
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP:
            {
                input::AddToQueue(message.wParam, message.lParam);
            }
            break;
        }

        TranslateMessage(&message);
        DispatchMessage(&message);
    }
}

Window::WindowDimensions Window::GetWindowDimensions()
{
    RECT windowRect;
    WindowDimensions dimensions;
    if(GetWindowRect(m_hwnd, &windowRect))
    {
        dimensions.width = windowRect.right - windowRect.left;
        dimensions.height = windowRect.bottom - windowRect.top;
    }
    else
    {
        OutputDebugStr(TEXT("Failed to retrieve window rect"));
    }
    return dimensions;
}


} // end namespace nc