#include <windows.h>
#include <stdint.h>
#include <memory>
#include <iostream>

#include "Display.hpp"
#include "../NCE/Engine/NCEMain.hpp"
#include "../NCE/Input/Input.hpp"


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
                break;
            } 

            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP:
            {
                uint32_t VKCode = message.wParam;
                
                NCE::Input::AddToQueue(VKCode, message.lParam);

                bool WasDown = ((message.lParam & (1 << 30)) != 0);
                bool IsDown  = ((message.lParam & (1 << 31)) == 0);

                if (IsDown != WasDown){
                    break;
                }
            }
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
            HDC deviceContext = BeginPaint(window, &paint);
            DisplayBufferInWindow(deviceContext, &renderBuffer);
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
    windowData.WindowClass.lpszClassName = TEXT("FirstClass");

    if (!RegisterClass(&windowData.WindowClass)){
        OutputDebugString(TEXT("WNDCLASS not registered\n"));
        return 0;
    }

    windowData.Window = CreateWindowExA(0,
                                        (LPCSTR)windowData.WindowClass.lpszClassName,
                                        "FirstClass",
                                        WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                        0, 0, SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2,
                                        0,
                                        0,
                                        instance,
                                        0);
    if (!windowData.Window){
        OutputDebugString(TEXT("Window handle not found\n"));
        return 0;
    }

    windowData.DeviceContext = GetDC(windowData.Window);

    windowDimensions = GetWindowDimensions(windowData.Window);
    InitializeRenderBuffer(&renderBuffer, SCREEN_WIDTH, SCREEN_HEIGHT);

    LoadSprite();

    NCE::Engine::InitializeEngine(SCREEN_WIDTH, SCREEN_HEIGHT, Win32ProcessSystemMessages, Render);
    NCE::Engine::NCEMain();

	return 0;
}



