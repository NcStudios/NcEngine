#include "VulkanGraphics.h"
#include "asset/NcAsset.h"
#include "config/Config.h"
#include "ecs/Registry.h"
#include "graphics/api/vulkan/FrameManager.h"
#include "graphics/api/vulkan/GpuAllocator.h"
#include "graphics/api/vulkan/Imgui.h"
#include "graphics/api/vulkan/RenderGraph.h"
#include "graphics/api/vulkan/ShaderBindingManager.h"
#include "graphics/api/vulkan/ShaderStorage.h"
#include "graphics/api/vulkan/Swapchain.h"
#include "graphics/api/vulkan/core/Device.h"
#include "graphics/api/vulkan/core/Instance.h"
#include "graphics/Camera.h"
#include "graphics/shader_resource/ShaderResourceBus.h"
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
      m_frameManager{std::make_unique<FrameManager>(*m_device)},
      m_shaderBindingManager{ std::make_unique<ShaderBindingManager>(m_device->VkDevice())},
      m_renderGraph{std::make_unique<RenderGraph>(m_frameManager.get(), registry, m_device.get(), m_swapchain.get(), m_allocator.get(), m_shaderBindingManager.get(), dimensions, memorySettings.maxPointLights + memorySettings.maxSpotLights)},
      m_shaderStorage{std::make_unique<ShaderStorage>(m_device->VkDevice(), m_allocator.get(), m_shaderBindingManager.get(), m_renderGraph.get(), m_frameManager->CommandBuffers(),
                                                      shaderResourceBus.cubeMapArrayBufferChannel, shaderResourceBus.meshArrayBufferChannel, shaderResourceBus.ppImageArrayBufferChannel,
                                                      shaderResourceBus.storageBufferChannel, shaderResourceBus.uniformBufferChannel, shaderResourceBus.textureArrayBufferChannel)},
      m_imgui{std::make_unique<Imgui>(*m_device, *m_instance, window, m_renderGraph->GetLitPass().GetVkPass(), assetModule->OnFontUpdate())},
      m_resizingMutex{},
      m_imageIndex{UINT32_MAX},
      m_dimensions{dimensions},
      m_screenExtent{screenExtent},
      m_isMinimized{false},
      m_resourceLayoutInitialized{false}
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
    m_device->VkDevice().waitIdle();
}

void VulkanGraphics::OnResize(const Vector2& dimensions, bool isMinimized)
{
    m_dimensions = dimensions;
    m_isMinimized = isMinimized;
    Resize(m_dimensions);
}

void VulkanGraphics::Clear() noexcept
{
    m_device->VkDevice().waitIdle();
    m_renderGraph->ClearShadowPasses();
    m_device->VkDevice().waitIdle();
}

bool VulkanGraphics::PrepareFrame()
{
    OPTICK_CATEGORY("VulkanGraphics::PrepareFrame", Optick::Category::Rendering);
    if (m_isMinimized) return false;

    // Gets the next image in the swapchain
    if(!m_swapchain->GetNextRenderReadyImageIndex(m_frameManager->CurrentFrameContext(), &m_imageIndex))
    {
        Resize(m_dimensions);
    }

    m_imgui->FrameBegin();
    return true;
}

bool VulkanGraphics::BeginFrame()
{
    OPTICK_CATEGORY("VulkanGraphics::BeginFrame", Optick::Category::Rendering);
    if (m_isMinimized) return false;
    m_frameManager->Begin();
    return true;
}

void VulkanGraphics::CommitResourceLayout()
{
    if (m_isMinimized)
    {
        return;
    }

    if (!m_resourceLayoutInitialized)
    {
        m_shaderBindingManager->CommitResourceLayout();
        m_resourceLayoutInitialized = true;
    }

    m_renderGraph->CommitResourceLayout();
    m_renderGraph->SinkPostProcessImages();
}

auto VulkanGraphics::CurrentFrameIndex() -> uint32_t
{
    return m_frameManager->Index();
}

// Executes the command buffer for the next swapchain image which writes to the image.
// Then, returns the image written to to the swap chain for presentation.
// Note: All calls below are asynchronous fire-and-forget methods. A maximum of MaxFramesInFlight sets of calls will be running at any given time.
void VulkanGraphics::DrawFrame(const PerFrameRenderState& state)
{
    OPTICK_CATEGORY("VulkanGraphics::Draw", Optick::Category::Rendering);
    if (m_isMinimized) return;

    m_imgui->Frame();

    // Executes the draw commands for the graph (recording them into the command buffer for the given frame)
    m_renderGraph->RecordDrawCallsOnBuffer(state, m_imageIndex, m_dimensions, m_screenExtent);
}

void VulkanGraphics::FrameEnd()
{
    auto* currentFrame = m_frameManager->CurrentFrameContext();
    currentFrame->CommandBuffer()->end();

    // Executes the command buffer to render to the image
    m_swapchain->WaitImageReadyForBuffer(currentFrame, m_imageIndex);
    currentFrame->SubmitBufferToQueue(m_device->VkGraphicsQueue());

    if (!m_swapchain->PresentImageToSwapChain(currentFrame, m_device->VkPresentQueue(), m_imageIndex))
    {
        Resize(m_dimensions);
    }

    m_frameManager->End();
}
} // namespace nc::graphics::vulkan
