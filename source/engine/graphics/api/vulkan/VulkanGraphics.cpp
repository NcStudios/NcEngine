#include "VulkanGraphics.h"
#include "FrameManager.h"
#include "GpuAllocator.h"
#include "GpuAssetsStorage.h"
#include "GpuShaderStorage.h"
#include "graphics/shader_resource/ShaderResourceBus.h"
#include "Imgui.h"
#include "Lighting.h"
#include "RenderGraph.h"
#include "Swapchain.h"
#include "core/Device.h"
#include "core/Instance.h"
#include "shaders/ShaderDescriptorSets.h"
#include "shaders/ShaderResources.h"
#include "asset/NcAsset.h"
#include "config/Config.h"
#include "ecs/Registry.h"
#include "graphics/Camera.h"
#include "utility/Log.h"

#include "ncutility/NcError.h"
#include "optick.h"

#include "GLFW/glfw3.h"

namespace
{
constexpr auto g_requiredDeviceExtensions = std::array<const char*, 1>{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
} // anonymous namespace

namespace nc::graphics::vulkan
{
VulkanGraphics::VulkanGraphics(const config::ProjectSettings& projectSettings,
                               const config::GraphicsSettings& graphicsSettings,
                               const config::MemorySettings& memorySettings,
                               asset::NcAsset* assetModule,
                               ShaderResourceBus& shaderResourceBus,
                               uint32_t apiVersion, Registry* registry,
                               GLFWwindow* window, Vector2 dimensions, Vector2 screenExtent)
    : m_instance{std::make_unique<Instance>(projectSettings.projectName, 1, apiVersion, graphicsSettings.useValidationLayers)},
      m_surface{m_instance->CreateSurface(window)},
      m_device{Device::Create(*m_instance, m_surface.get(), g_requiredDeviceExtensions)},
      m_swapchain{ std::make_unique<Swapchain>(*m_device, m_surface.get(), dimensions)},
      m_allocator{ std::make_unique<GpuAllocator>(m_device.get(), *m_instance)},
      m_shaderDescriptorSets{ std::make_unique<ShaderDescriptorSets>(m_device->VkDevice())},
      m_shaderResources{ std::make_unique<ShaderResources>(m_device->VkDevice(), m_shaderDescriptorSets.get(), m_allocator.get(), config::GetMemorySettings(), shaderResourceBus)},
      m_renderGraph{std::make_unique<RenderGraph>(m_device.get(), m_swapchain.get(), m_allocator.get(), m_shaderDescriptorSets.get(), dimensions, memorySettings.maxPointLights)},
      m_gpuAssetsStorage{ std::make_unique<GpuAssetsStorage>(m_device->VkDevice(), m_allocator.get(), assetModule->OnMeshUpdate()) },
      m_gpuShaderStorage{ std::make_unique<GpuShaderStorage>(m_device->VkDevice(), m_allocator.get(), m_shaderDescriptorSets.get(), shaderResourceBus.cubeMapArrayBufferChannel, shaderResourceBus.storageBufferChannel, shaderResourceBus.uniformBufferChannel, shaderResourceBus.textureArrayBufferChannel)},
      m_imgui{std::make_unique<Imgui>(*m_device, *m_instance, window, m_renderGraph->GetLitPass().GetVkPass())},
      m_frameManager{std::make_unique<FrameManager>(*m_device)},
      m_lighting{std::make_unique<Lighting>(registry, m_renderGraph.get(), m_shaderResources.get())},
      m_resizingMutex{},
      m_imageIndex{UINT32_MAX},
      m_dimensions{ dimensions },
      m_screenExtent{ screenExtent },
      m_isMinimized{ false }
{
}

VulkanGraphics::~VulkanGraphics() noexcept
{
    try
    {
        Clear();
    }
    catch (const std::runtime_error &e)
    {
        NC_LOG_EXCEPTION(e);
    }
}

void VulkanGraphics::Resize(const Vector2& dimensions)
{
    if (m_isMinimized)
    {
        return;
    }

    std::lock_guard lock{m_resizingMutex};

    // Wait for all current commands to complete execution
    m_device->VkDevice().waitIdle();
    m_dimensions = dimensions;
    m_swapchain->Resize(*m_device, dimensions);
    m_renderGraph->Resize(dimensions);
    m_lighting->Resize();
}

void VulkanGraphics::OnResize(float width, float height, bool isMinimized)
{
    m_dimensions = Vector2{ width, height };
    m_isMinimized = isMinimized;
    Resize(m_dimensions);
}

void VulkanGraphics::Clear() noexcept
{
    m_device->VkDevice().waitIdle();
    m_lighting->Clear();
}

bool VulkanGraphics::FrameBegin()
{
    OPTICK_CATEGORY("VulkanGraphics::FrameBegin", Optick::Category::Rendering);
    if (m_isMinimized) return false;

    // Gets the next image in the swapchain
    if(!m_swapchain->GetNextRenderReadyImageIndex(m_frameManager->CurrentFrameContext(), &m_imageIndex))
    {
        Resize(m_dimensions);
    }

    m_frameManager->Begin();
    m_imgui->FrameBegin();
    return true;
}

void VulkanGraphics::CommitResourceLayout()
{
    m_shaderDescriptorSets->CommitResourceLayout();
}

auto VulkanGraphics::CurrentFrameIndex() -> uint32_t
{
    return m_frameManager->Index();
}

// Executes the command buffer for the next swapchain image which writes to the image.
// Then, returns the image written to to the swap chain for presentation.
// Note: All calls below are asynchronous fire-and-forget methods. A maximum of MaxFramesInFlight sets of calls will be running at any given time.
void VulkanGraphics::Draw(const PerFrameRenderState& state)
{
    OPTICK_CATEGORY("VulkanGraphics::Draw", Optick::Category::Rendering);
    auto* currentFrame = m_frameManager->CurrentFrameContext();
    if (m_isMinimized) return;
    m_imgui->Frame();

    // Executes the draw commands for the graph (recording them into the command buffer for the given frame)
    m_renderGraph->Execute(currentFrame, state, m_gpuAssetsStorage.get()->meshStorage, m_imageIndex, m_dimensions, m_screenExtent, CurrentFrameIndex());

    // Executes the command buffer to render to the image
    m_swapchain->WaitForNextImage(currentFrame, m_imageIndex);
    currentFrame->RenderFrame(m_device->VkGraphicsQueue());
    m_device->VkDevice().waitIdle();

    // Returns the image to the swapchain
    bool isSwapChainValid = true;
    m_swapchain->Present(currentFrame, m_device->VkGraphicsQueue(), m_imageIndex, isSwapChainValid);

    if (!isSwapChainValid)
    {
        Resize(m_dimensions);
    }
}

void VulkanGraphics::FrameEnd()
{
    m_frameManager->End();
}
} // namespace nc::graphics::vulkan
