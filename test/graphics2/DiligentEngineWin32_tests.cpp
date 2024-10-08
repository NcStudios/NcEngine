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
                m_window = glfwCreateWindow(static_cast<uint32_t>(info.dimensions.x), static_cast<uint32_t>(info.dimensions.y), "DiligentEngineWin32_tests", nullptr, nullptr);
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

namespace
{
    void ExpectDiligentInitSucceeds(bool isHeadless, std::string targetApi, bool isGLWindowContext)
    {
        nc::graphics::ConditionalThrow = []() {};
        
        /* Create config */
        auto graphicsSettings = nc::config::GraphicsSettings();
        graphicsSettings.isHeadless = isHeadless;
        graphicsSettings.api = targetApi;
        auto projectSettings = nc::config::ProjectSettings();
        projectSettings.projectName = "DiligentEngineWin32_tests";

        /* Create window */
        auto info = nc::window::WindowInfo{ .isGL = isGLWindowContext, .isHeadless = graphicsSettings.isHeadless};
        auto ncWindow = nc::window::NcWindowImpl{info};

        /* Create DiligentEngine */
        auto supportedApis = nc::graphics::GetSupportedApis();
        EXPECT_NO_THROW(nc::graphics::DiligentEngine(graphicsSettings, ncWindow.GetWindowHandle(), supportedApis));
    }
} // anonymous namespace

using namespace nc::graphics;

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_VulkanNotHeadless_Succeeds)
{
    ExpectDiligentInitSucceeds(false, "vulkan", false);
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_VulkanHeadless_Succeeds)
{
    ExpectDiligentInitSucceeds(true, "vulkan", false);
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_OpenGLNotHeadless_Succeeds)
{
    ExpectDiligentInitSucceeds(false, "opengl", true);
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_OpenGLHeadless_Succeeds)
{
    ExpectDiligentInitSucceeds(true, "opengl", true);
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_D3D11LNotHeadless_Succeeds)
{
    ExpectDiligentInitSucceeds(false, "d3d11", false);
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_D3D11Headless_Succeeds)
{
    ExpectDiligentInitSucceeds(true, "d3d11", false);
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_D3D12LNotHeadless_Succeeds)
{
    ExpectDiligentInitSucceeds(false, "d3d12", false);
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_D3D12Headless_Succeeds)
{
    ExpectDiligentInitSucceeds(true, "d3d12", false);
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_APINotInSupportedApisList_Fails)
{
    /* Create config */
    auto graphicsSettings = nc::config::GraphicsSettings();
    graphicsSettings.isHeadless = false;
    graphicsSettings.api = "notInList";
    auto projectSettings = nc::config::ProjectSettings();
    projectSettings.projectName = "DiligentEngineWin32_tests";

    /* Create window */
    auto info = nc::window::WindowInfo{ .isGL = false, .isHeadless = graphicsSettings.isHeadless};
    auto ncWindow = nc::window::NcWindowImpl{info};

    std::vector<std::string_view> supportedApis =
    {
        "vulkan"
    };

    nc::graphics::ConditionalThrow = []() {};

    /* Create DiligentEngine */
    EXPECT_THROW(DiligentEngine(graphicsSettings, ncWindow.GetWindowHandle(), supportedApis), nc::NcError);
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_TargetAPIFails_FallbackSucceeds)
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

    std::vector<std::string_view> supportedApis =
    {
        "vulkan",
        "d3d12"
    };

    nc::graphics::ConditionalThrow = []()
    {
        throw std::runtime_error("Simulated failure for testing");
    };

    /* Create DiligentEngine */
    EXPECT_THROW(DiligentEngine(graphicsSettings, ncWindow.GetWindowHandle(), supportedApis), nc::NcError);
}