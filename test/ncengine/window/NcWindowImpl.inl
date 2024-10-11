#include "window/Window.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

namespace nc::window
{
class NcWindowImpl : public NcWindow
{
    public:
        NcWindowImpl(WindowInfo info)
        : m_window{nullptr}
        {
            if (!glfwInit())
                throw std::runtime_error("Could not init GLFW.");

            SetWindow(info);
        };

        ~NcWindowImpl()
        {
            if (m_window)
            {
                glfwDestroyWindow(m_window);
            }
            glfwTerminate();
        }

        void SetWindow(WindowInfo info) override 
        {
            if (m_window)
                glfwDestroyWindow(m_window);
            
            if (info.isHeadless)
            {
                glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
            }
            else
            {
                glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
            }

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            
            m_window = glfwCreateWindow(static_cast<uint32_t>(info.dimensions.x), static_cast<uint32_t>(info.dimensions.y), "DiligentEngineLinux_tests", nullptr, nullptr);
            if (!m_window)
            {
                throw std::runtime_error("Could not create window.");
            }
        };

        void DestroyWindow()
        {
            if (m_window)
                glfwDestroyWindow(m_window);
        }

        void ProcessSystemMessages() override {};

        auto GetWindowHandle() const noexcept -> GLFWwindow* 
        { 
            return m_window;
        }

    private:
        GLFWwindow* m_window;

};
} // namespace nc::window
