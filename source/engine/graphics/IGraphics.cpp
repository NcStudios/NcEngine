#include "IGraphics.h"
#include "api/vulkan/VulkanGraphics.h"
#include "utility/Log.h"
#include "window/WindowImpl.h"

#include "ncutility/NcError.h"

namespace nc::graphics
{
auto GraphicsFactory(const config::ProjectSettings& projectSettings,
                     const config::GraphicsSettings& graphicsSettings,
                     const GpuAccessorSignals& signals,
                     ShaderResourceBus& shaderResourceBus,
                     Registry* registry,
                     window::WindowImpl* window) -> std::unique_ptr<IGraphics>
{
    // TODO: #343 Provide an API/version switch in GraphicsSettings. Continue using Vulkan v1.2 for now.
    constexpr auto apiVersion = VK_API_VERSION_1_2;
    NC_LOG_TRACE("Creating VulkanGraphics");
    return std::make_unique<vulkan::VulkanGraphics>(projectSettings, graphicsSettings,
                                                    signals, shaderResourceBus,
                                                    apiVersion, registry,
                                                    window->GetHWND(),
                                                    window->GetHINSTANCE(),
                                                    window->GetDimensions()
    );
}
} // namespace nc::graphics
