#pragma once
#include <windows.h>
#include <string>
#include <memory>
#include "../../graphics/internal/Graphics.h"



#include <vector> //only for boxes

namespace nc::graphics::primitive { class Cube; }

namespace nc
{
    class Window
    {
        public:
            static Window* Instance;

            Window(HINSTANCE instance) noexcept;
            ~Window() noexcept;
            Window(const Window& other) = delete;
            Window(Window&& other) = delete;
            Window& operator=(const Window& other) = delete;
            Window& operator=(Window&& other) = delete;

            void OnWindowResize();
            graphics::internal::Graphics& GetGraphics();

            static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
            static void CopyBufferToScreen(void *buffer, BITMAPINFO &bufferInfo, int width, int height);
            static void ProcessSystemMessages();
        
        private:
            struct WindowDimensions { int width; int height; } m_windowDimensions;
            HWND m_hwnd;
            WNDCLASS m_wndClass;
            HDC m_deviceContext;
            HINSTANCE m_hInstance;
            std::unique_ptr<graphics::internal::Graphics> m_graphics;

            std::vector<std::unique_ptr<nc::graphics::primitive::Cube>> cubes;

            Window::WindowDimensions GetWindowDimensions();
    };

} //end namespace nc