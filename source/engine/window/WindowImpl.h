#pragma once

#include "ncmath/Vector.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include <functional>
#include <vector>

namespace nc::window
{
    class IOnResizeReceiver;

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
            void SetDimensions(int width, int height) noexcept;

            void BindGraphicsOnResizeCallback(std::function<void(float,float,float,float,bool)> callback) noexcept;
            void RegisterOnResizeReceiver(IOnResizeReceiver* receiver);
            void UnregisterOnResizeReceiver(IOnResizeReceiver* receiver) noexcept;
            void InvokeResizeReceivers(GLFWwindow* window, int width, int height);
            
            void PollEvents();

        private:
            static void ProcessKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
            static void ProcessMouseButtonEvent(GLFWwindow* window, int button, int action, int mods);
            static void ProcessMouseCursorPosEvent(GLFWwindow* window, double xPos, double yPos);
            static void ProcessMouseScrollEvent(GLFWwindow* window, double xOffset, double yOffset);
            static void ProcessResizeEvent(GLFWwindow* window, int width, int height);
            static void ProcessWindowCloseEvent(GLFWwindow* window);
            std::vector<IOnResizeReceiver*> m_onResizeReceivers;
            Vector2 m_dimensions;
            GLFWwindow* m_window;

            std::function<void(float,float,float,float,bool)> GraphicsOnResizeCallback;
            std::function<void()> EngineDisableRunningCallback;
    };
} // end namespace nc::window