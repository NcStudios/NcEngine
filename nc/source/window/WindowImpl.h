#pragma once

#include "platform/win32/NcWin32.h"
#include "math/Vector.h"

#include <functional>
#include <vector>

namespace nc
{
    namespace window { class IOnResizeReceiver; }
}

namespace nc::window
{
    class WindowImpl
    {
        public:
            WindowImpl(HINSTANCE instance);
            ~WindowImpl() noexcept;
            WindowImpl(const WindowImpl& other) = delete;
            WindowImpl(WindowImpl&& other) = delete;
            WindowImpl& operator=(const WindowImpl& other) = delete;
            WindowImpl& operator=(WindowImpl&& other) = delete;

            HWND GetHWND() const noexcept;
            HINSTANCE GetHINSTANCE() const noexcept;
            Vector2 GetDimensions() const noexcept;

            void BindGraphicsOnResizeCallback(std::function<void(float,float,float,float,WPARAM)> callback) noexcept;
            void BindGraphicsSetClearColorCallback(std::function<void(std::array<float, 4>)> callback) noexcept;
            void BindUICallback(std::function<LRESULT(HWND,UINT,WPARAM,LPARAM)> callback) noexcept;

            void RegisterOnResizeReceiver(IOnResizeReceiver* receiver);
            void UnregisterOnResizeReceiver(IOnResizeReceiver* receiver) noexcept;
            void OnResize(float width, float height, WPARAM windowArg);
            void SetClearColor(std::array<float, 4> color) noexcept;

            void ProcessSystemMessages();

            static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

        private:
            std::vector<IOnResizeReceiver*> m_onResizeReceivers;
            HWND m_hwnd;
            WNDCLASS m_wndClass;
            HINSTANCE m_hInstance;
            Vector2 m_dimensions;

            std::function<void(float,float,float,float,WPARAM)> GraphicsOnResizeCallback;
            std::function<void(std::array<float, 4>)> GraphicsSetClearColorCallback;
            std::function<LRESULT(HWND,UINT,WPARAM,LPARAM)> UIWndMessageCallback;
    };
} // end namespace nc::window