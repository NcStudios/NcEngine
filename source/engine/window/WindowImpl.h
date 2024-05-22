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

        void RegisterOnResizeReceiver(IOnResizeReceiver* receiver);
        void UnregisterOnResizeReceiver(IOnResizeReceiver* receiver) noexcept;
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
        Signal<>* m_quit;
};
} // end namespace window
} // end namespace nc
