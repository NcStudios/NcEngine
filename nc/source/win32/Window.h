#pragma once
#include "NcWin32.h"
#include <utility> //std::pair

namespace nc
{
    namespace config { struct Config; }
    namespace ui { class UISystem; }

    class Window
    {
        public:
            static Window* Instance;

            Window(HINSTANCE instance, const config::Config& config);
            ~Window() noexcept;
            Window(const Window& other) = delete;
            Window(Window&& other) = delete;
            Window& operator=(const Window& other) = delete;
            Window& operator=(Window&& other) = delete;

            HWND GetHWND() const noexcept;
            std::pair<int, int> GetWindowDimensions() const noexcept;

            void BindUISystem(ui::UISystem* ui);

            void OnWindowResize();
            void ProcessSystemMessages();

            static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
        
        private:
            HWND m_hwnd;
            WNDCLASS m_wndClass;
            HDC m_deviceContext;
            HINSTANCE m_hInstance;
            std::pair<int, int> m_windowDimensions; 
            ui::UISystem* m_ui;
    };

} //end namespace nc