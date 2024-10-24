#include "IGraphics.h"
#include "api/vulkan/VulkanGraphics.h"
#include "utility/Log.h"
#include "window/Window.h"

#include "ncutility/NcError.h"

namespace nc::graphics
{
auto GraphicsFactory(const config::ProjectSettings& projectSettings,
                     const config::GraphicsSettings& graphicsSettings,
                     asset::NcAsset* assetModule,
                     window::NcWindow& window) -> std::unique_ptr<IGraphics>
{
    // TODO: #343 Provide an API/version switch in GraphicsSettings. Continue using Vulkan v1.3 for now.
    constexpr auto apiVersion = VK_API_VERSION_1_3;
    NC_LOG_TRACE("Creating VulkanGraphics");
    return std::make_unique<vulkan::VulkanGraphics>(projectSettings, graphicsSettings, assetModule, apiVersion,
                                                    window.GetWindowHandle(), window.GetDimensions(), window.GetScreenExtent()
    );
}
} // namespace nc::graphics
