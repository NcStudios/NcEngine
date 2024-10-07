#include "gtest/gtest.h"
#include "config/Config.h"
#include "graphics2/diligent/DiligentEngine.h"
#include "graphics/NcGraphics.h"
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
                m_window = glfwCreateWindow(100, 100, "DiligentEngineWin32_tests", nullptr, nullptr);
                if (!m_window)
                {
                    throw std::runtime_error("Could not create window.");
                }
            };

            void ProcessSystemMessages() override
            {
                glfwPollEvents();
            };

            auto GetWindowHandle() const noexcept -> GLFWwindow* 
            { 
                return m_window;
            }

        private:
            GLFWwindow* m_window;

    };
} // namespace nc::window

using namespace nc::graphics;

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_VulkanNotHeadless_Succeeds)
{
    /* Create config */
    auto graphicsSettings = nc::config::GraphicsSettings();
    graphicsSettings.isHeadless = false;
    graphicsSettings.api = "vulkan";
    auto projectSettings = nc::config::ProjectSettings();
    projectSettings.projectName = "DiligentEngineWin32_tests";

    /* Create window */
    auto info = nc::window::WindowInfo{ .isGL = false, .isHeadless = graphicsSettings.isHeadless};
    auto ncWindow = nc::window::NcWindowImpl{info};

    /* Create DiligentEngine */
    auto supportedApis = GetSupportedApis();
    EXPECT_NO_THROW(DiligentEngine(graphicsSettings, ncWindow.GetWindowHandle(), supportedApis));
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_VulkanHeadless_Succeeds)
{
    /* Create config */
    auto graphicsSettings = nc::config::GraphicsSettings();
    graphicsSettings.isHeadless = true;
    graphicsSettings.api = "vulkan";
    auto projectSettings = nc::config::ProjectSettings();
    projectSettings.projectName = "DiligentEngineWin32_tests";

    /* Create window */
    auto info = nc::window::WindowInfo{ .isGL = false, .isHeadless = graphicsSettings.isHeadless};
    auto ncWindow = nc::window::NcWindowImpl{info};

    /* Create DiligentEngine */
    auto supportedApis = GetSupportedApis();
    EXPECT_NO_THROW(DiligentEngine(graphicsSettings, ncWindow.GetWindowHandle(), supportedApis));
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_OpenGLNotHeadless_Succeeds)
{
    /* Create config */
    auto graphicsSettings = nc::config::GraphicsSettings();
    graphicsSettings.isHeadless = false;
    graphicsSettings.api = "opengl";
    auto projectSettings = nc::config::ProjectSettings();
    projectSettings.projectName = "DiligentEngineWin32_tests";

    /* Create window */
    auto info = nc::window::WindowInfo{ .isGL = false, .isHeadless = graphicsSettings.isHeadless};
    auto ncWindow = nc::window::NcWindowImpl{info};

    /* Create DiligentEngine */
    auto supportedApis = GetSupportedApis();
    EXPECT_NO_THROW(DiligentEngine(graphicsSettings, ncWindow.GetWindowHandle(), supportedApis));
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_OpenGLNotHeadless_BadWindow_Fails)
{
    /* Create config */
    auto graphicsSettings = nc::config::GraphicsSettings();
    graphicsSettings.isHeadless = false;
    graphicsSettings.api = "opengl";
    auto projectSettings = nc::config::ProjectSettings();
    projectSettings.projectName = "DiligentEngineWin32_tests";

    /* Create window. Set .isGL to false which will set GLFW_NO_API - this will cause creation of the GL context to fail. */
    auto info = nc::window::WindowInfo{ .isGL = false, .isHeadless = graphicsSettings.isHeadless};
    auto ncWindow = nc::window::NcWindowImpl{info};

    /* Create DiligentEngine */
    auto supportedApis = GetSupportedApis();
    EXPECT_THROW(DiligentEngine(graphicsSettings, ncWindow.GetWindowHandle(), supportedApis), std::runtime_error);
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_OpenGLHeadless_Succeeds)
{
    /* Create config */
    auto graphicsSettings = nc::config::GraphicsSettings();
    graphicsSettings.isHeadless = true;
    graphicsSettings.api = "opengl";
    auto projectSettings = nc::config::ProjectSettings();
    projectSettings.projectName = "DiligentEngineWin32_tests";

    /* Create window */
    auto info = nc::window::WindowInfo{ .isGL = false, .isHeadless = graphicsSettings.isHeadless};
    auto ncWindow = nc::window::NcWindowImpl{info};

    /* Create DiligentEngine */
    auto supportedApis = GetSupportedApis();
    EXPECT_NO_THROW(DiligentEngine(graphicsSettings, ncWindow.GetWindowHandle(), supportedApis));
}
