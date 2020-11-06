#pragma once
#include "NcWin32.h"
#include <utility> //std::pair

namespace nc
{
    namespace config { struct Config; }
    namespace graphics { class Graphics; }
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
            std::pair<unsigned, unsigned> GetWindowDimensions() const;

            void BindGraphics(graphics::Graphics* graphics);
            void BindUISystem(ui::UISystem* ui);

            void OnResize(unsigned width, unsigned height);
            void ProcessSystemMessages();

            static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
        
        private:
            HWND m_hwnd;
            WNDCLASS m_wndClass;
            HINSTANCE m_hInstance;
            graphics::Graphics* m_graphics;
            ui::UISystem* m_ui;

            std::pair<unsigned, unsigned> m_dimensions;
    };

} //end namespace nc