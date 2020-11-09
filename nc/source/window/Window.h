#pragma once
#include "win32/NcWin32.h"
#include "math/Vector2.h"

namespace nc
{
    namespace config { struct Config; }
    namespace engine { class Engine; }
    namespace graphics { class Graphics; }
    namespace ui { class UISystem; }

    class Window
    {
        public:
            static Window* Instance;

            Window(HINSTANCE instance, engine::Engine* engine, const config::Config& config);
            ~Window() noexcept;
            Window(const Window& other) = delete;
            Window(Window&& other) = delete;
            Window& operator=(const Window& other) = delete;
            Window& operator=(Window&& other) = delete;

            HWND GetHWND() const noexcept;
            Vector2 GetWindowDimensions() const;

            void BindGraphics(graphics::Graphics* graphics);
            void BindUISystem(ui::UISystem* ui);

            void OnResize(float width, float height);
            void ProcessSystemMessages();

            static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
        
        private:
            HWND m_hwnd;
            WNDCLASS m_wndClass;
            HINSTANCE m_hInstance;
            engine::Engine* m_engine;
            graphics::Graphics* m_graphics;
            ui::UISystem* m_ui;

            Vector2 m_dimensions;
    };

} //end namespace nc