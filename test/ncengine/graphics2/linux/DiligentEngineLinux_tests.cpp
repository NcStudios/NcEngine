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

namespace
{
    void ExpectDiligentInitSucceeds(bool isHeadless, std::string targetApi, bool isGLWindowContext)
    {
        /* Create config */
        auto graphicsSettings = nc::config::GraphicsSettings();
        graphicsSettings.isHeadless = isHeadless;
        graphicsSettings.api = targetApi;
        auto projectSettings = nc::config::ProjectSettings();
        projectSettings.projectName = "DiligentEngineLinux_tests";

        /* Create window */
        auto info = nc::window::WindowInfo{ .isGL = isGLWindowContext, .isHeadless = graphicsSettings.isHeadless};
        auto ncWindow = nc::window::NcWindowImpl{info};

        /* Create DiligentEngine */
        auto supportedApis = nc::graphics::GetSupportedApis();
        Diligent::EngineCreateInfo engineCI{};
        EXPECT_NO_THROW(nc::graphics::DiligentEngine(graphicsSettings, engineCI, ncWindow.GetWindowHandle(), supportedApis));
    }
} // anonymous namespace

using namespace nc::graphics;

TEST(DiligentEngineLinux_tests, CreateDiligentEngine_VulkanNotHeadless_Succeeds)
{
    ExpectDiligentInitSucceeds(false, "vulkan", false);
}

TEST(DiligentEngineLinux_tests, CreateDiligentEngine_VulkanHeadless_Succeeds)
{
    ExpectDiligentInitSucceeds(true, "vulkan", false);
}

TEST(DiligentEngineLinux_tests, CreateDiligentEngine_OpenGLNotHeadless_Succeeds)
{
    ExpectDiligentInitSucceeds(false, "opengl", true);
}

TEST(DiligentEngineLinux_tests, CreateDiligentEngine_OpenGLHeadless_Succeeds)
{
    ExpectDiligentInitSucceeds(true, "opengl", true);
}

TEST(DiligentEngineLinux_tests, CreateDiligentEngine_APINotInSupportedApisList_Fails)
{
    /* Create config */
    auto graphicsSettings = nc::config::GraphicsSettings();
    graphicsSettings.isHeadless = false;
    graphicsSettings.api = "d3d12";
    auto projectSettings = nc::config::ProjectSettings();
    projectSettings.projectName = "DiligentEngineLinux_tests";

    /* Create window */
    auto info = nc::window::WindowInfo{ .isGL = false, .isHeadless = graphicsSettings.isHeadless};
    auto ncWindow = nc::window::NcWindowImpl{info};

    std::vector<std::string_view> supportedApis =
    {
        "opengl"
    };

    /* Create DiligentEngine */
    Diligent::EngineCreateInfo engineCI{};
    EXPECT_THROW(DiligentEngine(graphicsSettings, engineCI, ncWindow.GetWindowHandle(), supportedApis), nc::NcError);
}

TEST(DiligentEngineLinux_tests, CreateDiligentEngine_TargetAPIFails_FallbackIsAttemptedAndFails)
{
    /* Create config */
    auto graphicsSettings = nc::config::GraphicsSettings();
    graphicsSettings.isHeadless = false;
    graphicsSettings.api = "vulkan";
    auto projectSettings = nc::config::ProjectSettings();
    projectSettings.projectName = "DiligentEngineLinux_tests";

    /* Create window */
    auto info = nc::window::WindowInfo{ .isGL = false, .isHeadless = graphicsSettings.isHeadless};
    auto ncWindow = nc::window::NcWindowImpl{info};

    std::vector<std::string_view> supportedApis =
    {
        "vulkan",
        "opengl"
    };

    /* Create DiligentEngine */
    Diligent::EngineCreateInfo engineCI{};
    engineCI.EngineAPIVersion = 500;
    try 
    {
        DiligentEngine(graphicsSettings, engineCI, ncWindow.GetWindowHandle(), supportedApis);
    }
    catch (const std::runtime_error& error)
    {
        auto errorStr = std::string_view(error.what());
        ASSERT_EQ(errorStr.contains("Failed to create the Vulkan device or context"), true);
        ASSERT_EQ(errorStr.contains("Failed to create the OpenGL device or context."), true);
        ASSERT_EQ(errorStr.contains("Failed to initialize the rendering engine. The given API and all fallback APIs failed to initialize."), true);
    }
}