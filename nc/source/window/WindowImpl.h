#pragma once

#include "win32/NcWin32.h"
#include "math/Vector2.h"

namespace nc
{
    namespace config { struct Config; }
    namespace engine { class Engine; }
    namespace graphics { class Graphics; }
    namespace ui { class UISystem; }
}

namespace nc::window
{
    class WindowImpl
    {
        public:
            WindowImpl(HINSTANCE instance, engine::Engine* engine, const config::Config& config);
            ~WindowImpl() noexcept;
            WindowImpl(const WindowImpl& other) = delete;
            WindowImpl(WindowImpl&& other) = delete;
            WindowImpl& operator=(const WindowImpl& other) = delete;
            WindowImpl& operator=(WindowImpl&& other) = delete;

            HWND GetHWND() const noexcept;
            Vector2 GetDimensions() const;

            void BindGraphics(graphics::Graphics* graphics);
            void BindUISystem(ui::UISystem* ui);

            void OnResize(float width, float height);
            void ProcessSystemMessages();

            static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

        private:
            static WindowImpl* m_instance;
            HWND m_hwnd;
            WNDCLASS m_wndClass;
            HINSTANCE m_hInstance;
            engine::Engine* m_engine;
            graphics::Graphics* m_graphics;
            ui::UISystem* m_ui;
            Vector2 m_dimensions;
    };
} // end namespace nc::window