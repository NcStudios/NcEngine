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

class NcWindowImpl : public NcWindow
{
    public:
        NcWindowImpl(const config::ProjectSettings& projectSettings,
                     bool isGraphicsEnabled,
                     Signal<>& quit);
        ~NcWindowImpl() noexcept;
        NcWindowImpl(const NcWindowImpl& other) = delete;
        NcWindowImpl(NcWindowImpl&& other) = delete;
        NcWindowImpl& operator=(const NcWindowImpl& other) = delete;
        NcWindowImpl& operator=(NcWindowImpl&& other) = delete;

        void RegisterOnResizeReceiver(IOnResizeReceiver* receiver);
        void UnregisterOnResizeReceiver(IOnResizeReceiver* receiver) noexcept;
        void SetWindow(WindowInfo windowInfo) override;
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

        std::string m_windowName;
        std::vector<IOnResizeReceiver*> m_onResizeReceivers;
        Signal<>* m_quit;
};
} // end namespace window
} // end namespace nc
