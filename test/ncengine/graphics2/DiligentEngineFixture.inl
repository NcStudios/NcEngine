#include "config/Config.h"
#include "graphics2/diligent/DiligentEngine.h"
#include "graphics/NcGraphics.h"
#include "../window/NcWindowImpl.inl"

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
