#pragma once

#include "win32/NcWin32.h"
#include "math/Vector2.h"

#include <functional>

namespace nc
{
    namespace config { struct Config; }
    namespace engine { class Engine; }
    namespace graphics { class Graphics; }
}

namespace nc::window
{
    class WindowImpl
    {
        public:
            WindowImpl(HINSTANCE instance,
                       const config::Config& config,
                       std::function<void(bool)> engineShutdownCallback);
            ~WindowImpl() noexcept;
            WindowImpl(const WindowImpl& other) = delete;
            WindowImpl(WindowImpl&& other) = delete;
            WindowImpl& operator=(const WindowImpl& other) = delete;
            WindowImpl& operator=(WindowImpl&& other) = delete;

            HWND GetHWND() const noexcept;
            Vector2 GetDimensions() const;

            void BindGraphicsOnResizeCallback(std::function<void(float,float,float,float)> callback);
            void BindUICallback(std::function<LRESULT(HWND,UINT,WPARAM,LPARAM)> callback);

            void OnResize(float width, float height);
            void ProcessSystemMessages();

            static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

        private:
            static WindowImpl* m_instance;
            HWND m_hwnd;
            WNDCLASS m_wndClass;
            HINSTANCE m_hInstance;
            Vector2 m_dimensions;

            std::function<void(bool)> EngineShutdownCallback;
            std::function<void(float,float,float,float)> GraphicsOnResizeCallback;
            std::function<LRESULT(HWND,UINT,WPARAM,LPARAM)> UIWndMessageCallback;
    };
} // end namespace nc::window