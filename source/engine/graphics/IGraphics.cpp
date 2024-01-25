#include "IGraphics.h"
#include "api/vulkan/VulkanGraphics.h"
#include "utility/Log.h"
#include "window/WindowImpl.h"

#include "ncutility/NcError.h"

namespace nc::graphics
{
auto GraphicsFactory(const config::ProjectSettings& projectSettings,
                     const config::GraphicsSettings& graphicsSettings,
                     asset::NcAsset* assetModule,
                     ShaderResourceBus& shaderResourceBus,
                     Registry* registry,
                     window::WindowImpl* window) -> std::unique_ptr<IGraphics>
{
    // TODO: #343 Provide an API/version switch in GraphicsSettings. Continue using Vulkan v1.1 for now.
    constexpr auto apiVersion = VK_API_VERSION_1_1;
    NC_LOG_TRACE("Creating VulkanGraphics");
    return std::make_unique<vulkan::VulkanGraphics>(projectSettings, graphicsSettings,
                                                    assetModule, shaderResourceBus,
                                                    apiVersion, registry,
                                                    window->GetWindow(),
                                                    window->GetDimensions(),
                                                    window->GetScreenExtent()
    );
}
} // namespace nc::graphics
