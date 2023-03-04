#include "VulkanGraphics.h"
#include "graphics/FrameManager.h"
#include "graphics/GpuAllocator.h"
#include "graphics/GpuAssetsStorage.h"
#include "graphics/GpuOptions.h"
#include "graphics/Imgui.h"
#include "graphics/Lighting.h"
#include "graphics/RenderGraph.h"
#include "graphics/Swapchain.h"
#include "graphics/core/Device.h"
#include "graphics/core/Instance.h"
#include "graphics/shaders/ShaderDescriptorSets.h"
#include "graphics/shaders/ShaderResources.h"

#include "config/Config.h"
#include "ecs/Registry.h"
#include "graphics/Camera.h"
#include "utility/Log.h"

#include "ncutility/NcError.h"
#include "optick/optick.h"

namespace
{
constexpr auto g_requiredDeviceExtensions = std::array<const char*, 1>{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
} // anonymous namespace

namespace nc::graphics::vulkan
{
VulkanGraphics::VulkanGraphics(const config::ProjectSettings& projectSettings,
                               const config::GraphicsSettings& graphicsSettings,
                               const GpuAccessorSignals& gpuAccessorSignals,
                               uint32_t apiVersion, Registry* registry, HWND hwnd,
                               HINSTANCE hinstance, Vector2 dimensions)
    : m_instance{std::make_unique<Instance>(projectSettings.projectName, 1, apiVersion, graphicsSettings.useValidationLayers)},
      m_surface{m_instance->CreateSurface(hwnd, hinstance)},
      m_device{Device::Create(*m_instance, m_surface.get(), g_requiredDeviceExtensions)},
      m_gpuOptions{ std::make_unique<GpuOptions>(m_device->VkPhysicalDevice()) },
      m_swapchain{ std::make_unique<Swapchain>(m_device->VkDevice(), m_device->VkPhysicalDevice(), m_surface.get(), dimensions)},
      m_allocator{ std::make_unique<GpuAllocator>(m_device.get(), *m_instance)},
      m_shaderDescriptorSets{ std::make_unique<ShaderDescriptorSets>(m_device->VkDevice())},
      m_shaderResources{ std::make_unique<ShaderResources>(m_device->VkDevice(), m_shaderDescriptorSets.get(), m_allocator.get(), config::GetMemorySettings())},
      m_gpuAssetsStorage{ std::make_unique<GpuAssetsStorage>(m_device->VkDevice(), m_allocator.get(), gpuAccessorSignals) },
      m_renderGraph{std::make_unique<RenderGraph>(m_device->VkDevice(), m_swapchain.get(), m_gpuOptions.get(), m_allocator.get(), m_shaderDescriptorSets.get(), dimensions)},
      m_imgui{std::make_unique<Imgui>(*m_device)},
      m_frameManager{std::make_unique<FrameManager>(m_device->VkDevice(), m_device->VkPhysicalDevice(), m_surface.get())},
      m_lighting{std::make_unique<Lighting>(registry, m_device->VkDevice(), m_allocator.get(), m_gpuOptions.get(), m_swapchain.get(), m_renderGraph.get(), m_shaderDescriptorSets.get(), m_shaderResources.get(), dimensions)},
      m_resizingMutex{},
      m_imageIndex{UINT32_MAX},
      m_dimensions{ dimensions },
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
    m_swapchain->Resize(dimensions);
    m_renderGraph->Resize(dimensions);
    m_lighting->Resize(dimensions);
}

void VulkanGraphics::OnResize(float width, float height, const WPARAM windowArg)
{
    m_dimensions = Vector2{ width, height };
    m_isMinimized = windowArg == 1;
    Resize(m_dimensions);
}

void VulkanGraphics::Clear() noexcept
{
    m_device->VkDevice().waitIdle();
    m_lighting->Clear();
    ShaderResourceService<ObjectData>::Get()->Reset();
    ShaderResourceService<PointLightInfo>::Get()->Reset();
    ShaderResourceService<ShadowMap>::Get()->Reset();
    ShaderResourceService<EnvironmentData>::Get()->Reset();
}

void VulkanGraphics::InitializeUI() /** @todo: I hate this whole implementation of ImGui and want to create an abstraction layer for it. */
{
    m_imgui->InitializeImgui(*m_instance, *m_device,
        m_renderGraph->GetRenderPass(LitPassId).GetVkPass(),
        static_cast<uint32_t>(m_gpuOptions->GetMaxSamplesCount())
    );
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
    return true;
}

// Executes the command buffer for the next swapchain image which writes to the image.
// Then, returns the image written to to the swap chain for presentation.
// Note: All calls below are asynchronous fire-and-forget methods. A maximum of MaxFramesInFlight sets of calls will be running at any given time.
void VulkanGraphics::Draw(const PerFrameRenderState& state)
{
    OPTICK_CATEGORY("VulkanGraphics::Draw", Optick::Category::Rendering);
    auto* currentFrame = m_frameManager->CurrentFrameContext();
    if (m_isMinimized) return;

    // Executes the draw commands for the graph (recording them into the command buffer for the given frame)
    m_renderGraph->Execute(currentFrame, state, m_gpuAssetsStorage.get()->meshStorage, m_imageIndex, m_dimensions);

    // Executes the command buffer to render to the image
    m_swapchain->WaitForNextImage(currentFrame, m_imageIndex);
    currentFrame->RenderFrame(m_device->VkGraphicsQueue());

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