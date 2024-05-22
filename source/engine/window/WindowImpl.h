#pragma once

#include "ncengine/window/Window.h"

#include "ncmath/Vector.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include <functional>
#include <vector>

namespace nc
{
namespace config
{
struct GraphicsSettings;
struct ProjectSettings;
} // namespace config

namespace window
{
class IOnResizeReceiver;

class WindowImpl : public NcWindow
{
    public:
        WindowImpl(const config::ProjectSettings& projectSettings,
                   const config::GraphicsSettings& graphicsSettings,
                   Signal<>& quit);
        ~WindowImpl() noexcept;
        WindowImpl(const WindowImpl& other) = delete;
        WindowImpl(WindowImpl&& other) = delete;
        WindowImpl& operator=(const WindowImpl& other) = delete;
        WindowImpl& operator=(WindowImpl&& other) = delete;

        auto GetWindowHandle() const noexcept -> GLFWwindow* override { return m_window; }
        auto GetDimensions() const noexcept -> const Vector2& override { return m_dimensions; }
        auto GetScreenExtent() const noexcept -> const Vector2& override { return m_screenExtent; }
        auto GetContentScale() const noexcept -> const Vector2& override { return m_contentScale; }
        auto OnResize() noexcept -> Signal<float, float, bool>& override { return m_onResize; }

        // void BindGraphicsOnResizeCallback(std::function<void(float,float,bool)> callback) noexcept override;
        void RegisterOnResizeReceiver(IOnResizeReceiver* receiver);
        void UnregisterOnResizeReceiver(IOnResizeReceiver* receiver) noexcept;
        void InvokeResizeReceivers(GLFWwindow* window, int width, int height);

        void ProcessSystemMessages() override;

    private:
        void SetDimensions(int width, int height) noexcept;

        static void ProcessKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void ProcessMouseButtonEvent(GLFWwindow* window, int button, int action, int mods);
        static void ProcessMouseCursorPosEvent(GLFWwindow* window, double xPos, double yPos);
        static void ProcessMouseScrollEvent(GLFWwindow* window, double xOffset, double yOffset);
        static void ProcessResizeEvent(GLFWwindow* window, int width, int height);
        static void ProcessSetContentScaleEvent(GLFWwindow* window, float x, float y);
        static void ProcessWindowCloseEvent(GLFWwindow* window);

        std::vector<IOnResizeReceiver*> m_onResizeReceivers;
        Vector2 m_dimensions;
        Vector2 m_screenExtent;
        Vector2 m_contentScale;
        GLFWwindow* m_window;
        Signal<float, float, bool> m_onResize;
        Signal<>* EngineDisableRunningCallback;
};
} // end namespace window
} // end namespace nc
