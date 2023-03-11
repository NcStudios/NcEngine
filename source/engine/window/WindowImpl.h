#pragma once

#include "platform/win32/NcWin32.h"
#include "ncmath/Vector.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

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
            WindowImpl(std::function<void()> onQuit);
            ~WindowImpl() noexcept;
            WindowImpl(const WindowImpl& other) = delete;
            WindowImpl(WindowImpl&& other) = delete;
            WindowImpl& operator=(const WindowImpl& other) = delete;
            WindowImpl& operator=(WindowImpl&& other) = delete;

            auto GetWindow() -> GLFWwindow*;
            auto GetDimensions() const noexcept -> Vector2;

            void BindGraphicsOnResizeCallback(std::function<void(float,float,float,float,WPARAM)> callback) noexcept;
            void BindUICallback(std::function<LRESULT(HWND,UINT,WPARAM,LPARAM)> callback) noexcept;

            void RegisterOnResizeReceiver(IOnResizeReceiver* receiver);
            void UnregisterOnResizeReceiver(IOnResizeReceiver* receiver) noexcept;
            void OnResize(float width, float height, WPARAM windowArg);
            
            void PollEvents();
            static void ProcessKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
            static void ProcessMouseButtonEvent(GLFWwindow* window, int button, int action, int mods);
            static void ProcessMouseCursorPosEvent(GLFWwindow* window, double xPos, double yPos);
            static void ProcessMouseScrollEvent(GLFWwindow* window, double xOffset, double yOffset);

        private:
            std::vector<IOnResizeReceiver*> m_onResizeReceivers;
            Vector2 m_dimensions;
            GLFWwindow* m_window;

            std::function<void(float,float,float,float,WPARAM)> GraphicsOnResizeCallback;
            std::function<LRESULT(HWND,UINT,WPARAM,LPARAM)> UIWndMessageCallback;
            std::function<void()> EngineDisableRunningCallback;
    };
} // end namespace nc::window