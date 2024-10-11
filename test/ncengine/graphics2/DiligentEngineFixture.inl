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

auto CreateDiligentEngine(bool isHeadless, std::string_view targetApi, std::span<const std::string_view> supportedApis, Diligent::EngineCreateInfo engineCI, nc::window::NcWindowImpl* window = nullptr) -> nc::graphics::DiligentEngine
{
    /* Create config */
    auto graphicsSettings = nc::config::GraphicsSettings();
    graphicsSettings.isHeadless = isHeadless;
    graphicsSettings.api = targetApi;
    auto projectSettings = nc::config::ProjectSettings();
    projectSettings.projectName = "DiligentEngineLinux_tests";

    if (!window)
    {
        /* Create window */
        auto info = nc::window::WindowInfo{.isHeadless = graphicsSettings.isHeadless};
        auto ncWindow = nc::window::NcWindowImpl{info};

        /* Create DiligentEngine */
        return nc::graphics::DiligentEngine(graphicsSettings, engineCI, ncWindow.GetWindowHandle(), supportedApis);
    }

    /* Create DiligentEngine */
     return nc::graphics::DiligentEngine(graphicsSettings, engineCI, window->GetWindowHandle(), supportedApis);
}

auto GetFullApiName(const std::string_view api) -> std::string_view
{
    if (api == "vulkan")
    {
        return "Vulkan";
    }
    if (api == "d3d12")
    {
        return "D3D12";
    }
    if (api == "d3d11")
    {
        return "D3D11";
    }

    throw std::runtime_error("API not in list.");
}

std::vector<std::string_view> ExcludeElementFromContainer(std::span<const std::string_view> inputSpan, const std::string_view& elementToExclude)
{
    std::vector<std::string_view> result;
    result.reserve(inputSpan.size());

    for (const auto& elem : inputSpan)
    {
        if (elem != elementToExclude)
        {
            result.push_back(elem);
        }
    }

    return result;
}
