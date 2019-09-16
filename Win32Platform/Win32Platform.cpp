#include <windows.h>
#include <stdint.h>
#include <memory>
#include <iostream>

#include "../NCE/Engine/NCEMain.hpp"
#include "../NCE/Input/Input.hpp"

struct WindowDimensions
{
    int width;
    int height;
};

struct WindowData
{
    WNDCLASS WindowClass;
    HWND Window;
    HDC DeviceContext;
};

//Temp - should be read at start up
const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 576;

WindowDimensions windowDimensions;
WindowData windowData;


WindowDimensions GetWindowDimensions(HWND t_hwnd)
{
    RECT windowRect;
    WindowDimensions dimensions;

    if (GetWindowRect(t_hwnd, &windowRect))
    {
        dimensions.width  = windowRect.right - windowRect.left;
        dimensions.height = windowRect.bottom - windowRect.top;
    }
    else
    {
        OutputDebugStr(TEXT("Failed to retrieve window rect."));
    }
    return dimensions;
}


void OnWindowResize(HWND hwnd)
{
    windowDimensions = GetWindowDimensions(hwnd);

    PatBlt(windowData.DeviceContext,
           0, 0, windowDimensions.width, windowDimensions.height,
           BLACKNESS);
}

void CopyBufferToScreen(void *t_buffer, BITMAPINFO &t_bufferInfo, int t_srcWidth, int t_srcHeight)
{
    HDC deviceContext = GetDC(windowData.Window);
    int xOffset = (windowDimensions.width - t_srcWidth) / 2;
    int yOffset = (windowDimensions.height - t_srcHeight) / 2;

    StretchDIBits(deviceContext,                             //destination (screen)
                  xOffset, yOffset, t_srcWidth, t_srcHeight, //destination x, y, w, h
                  0, 0, t_srcWidth, t_srcHeight,             //source x, y, w, h
                  t_buffer, &t_bufferInfo,                   //source buffer, buffer formatting   
                  DIB_RGB_COLORS, SRCCOPY);                  //RGB data flag, copy bits flag                            
}

void Win32ProcessSystemMessages()
{
    MSG message;
    while(PeekMessage(&message, 0, 0, 0, PM_REMOVE))
    {
        switch (message.message)
        {
            case WM_QUIT:
            {
                NCE::Engine::Exit();
            } 
            break;

            case WM_MOUSEMOVE:
            {
                NCE::Input::UpdateMousePosition(message.lParam);
            }
            break;

            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP:
            {
                NCE::Input::AddToQueue(message.wParam, message.lParam);
            }
            break;
        }

        TranslateMessage(&message);
        DispatchMessage(&message);
    }
}


LRESULT CALLBACK WndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
        case WM_CREATE:
            
            break;
        case WM_SIZE:
        {
            OnWindowResize(window);
        }
        break;

        case WM_CLOSE:
            NCE::Engine::Exit();
            break;
        case WM_ACTIVATEAPP:

            break;
        case WM_DESTROY:
            NCE::Engine::Exit();
            break;

        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            //HDC deviceContext = BeginPaint(window, &paint);
            BeginPaint(window, &paint);
            //CopyBufferToScreen(deviceContext, &renderBuffer);
            NCE::Engine::ForceRender();
            EndPaint(window, &paint);
        }
        break;

        default:
            return DefWindowProc(window, message, wParam, lParam);
	}
	return 0;
}




int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCommand )
{
    windowData.WindowClass = {}; 
    windowData.WindowClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;  //OWNDC allows device context to be retreived once rather than get/dispose each frame
	windowData.WindowClass.lpfnWndProc = WndProc;                   //CALLBACK that windows will call for handling messages
    windowData.WindowClass.hInstance = instance;                    //for windows to callback to the function it must also know about our process instance
    windowData.WindowClass.lpszClassName = TEXT("NCEngine - Test Project");

    if (!RegisterClass(&windowData.WindowClass)){
        OutputDebugString(TEXT("WNDCLASS not registered\n"));
        return 0;
    }

    windowData.Window = CreateWindowExA(0,
                                        (LPCSTR)windowData.WindowClass.lpszClassName,
                                        "NCEngine - Test Project",
                                        WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                        0, 0, SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2,
                                        0, 0, instance, 0);
    if (!windowData.Window){
        OutputDebugString(TEXT("Window handle not found\n"));
        return 0;
    }

    windowData.DeviceContext = GetDC(windowData.Window);
    windowDimensions = GetWindowDimensions(windowData.Window);

    NCE::Engine::InitializeEngine(SCREEN_WIDTH, SCREEN_HEIGHT, Win32ProcessSystemMessages);
    NCE::Engine::InitializeRenderer(CopyBufferToScreen);
    NCE::Engine::NCEMain();

	return 0;
}



