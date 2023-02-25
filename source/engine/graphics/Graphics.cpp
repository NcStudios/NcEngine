#include "Graphics.h"
#include "FrameManager.h"
#include "GpuAssetsStorage.h"
#include "GpuOptions.h"
#include "Renderer.h"
#include "Swapchain.h"
#include "core/Device.h"
#include "core/Instance.h"
#include "shaders/ShaderResources.h"
#include "config/Config.h"
#include "graphics/Camera.h"
#include "utility/Log.h"

#include "ncutility/NcError.h"
#include "optick/optick.h"

namespace
{
constexpr auto g_requiredDeviceExtensions = std::array<const char*, 1>{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
} // anonymous namespace

namespace nc::graphics
{
    Graphics::Graphics(const nc::GpuAccessorSignals& gpuAccessorSignals,
                       const std::string& appName, uint32_t appVersion, uint32_t apiVersion,
                       bool useValidationLayers, HWND hwnd, HINSTANCE hinstance, Vector2 dimensions)
        : m_instance{std::make_unique<Instance>(appName, appVersion, apiVersion, useValidationLayers)},
          m_surface{m_instance->CreateSurface(hwnd, hinstance)},
          m_device{Device::Create(*m_instance, m_surface.get(), g_requiredDeviceExtensions)},
          m_gpuOptions{ std::make_unique<GpuOptions>(m_device->VkPhysicalDevice()) },
          m_swapchain{ std::make_unique<Swapchain>(m_device->VkDevice(), m_device->VkPhysicalDevice(), m_surface.get(), dimensions) },
          m_allocator{ std::make_unique<GpuAllocator>(m_device.get(), *m_instance)},
          m_shaderResources{ std::make_unique<ShaderResources>(m_device->VkDevice(), m_allocator.get(), config::GetMemorySettings(), dimensions)},
          m_gpuAssetsStorage{ std::make_unique<GpuAssetsStorage>(m_device->VkDevice(), m_allocator.get(), gpuAccessorSignals) },
          m_renderer{ std::make_unique<Renderer>(m_device->VkDevice(), m_swapchain.get(), m_gpuOptions.get(), m_allocator.get(), m_shaderResources.get(), dimensions) },
          m_frameManager{std::make_unique<FrameManager>(m_device->VkDevice(), m_device->VkPhysicalDevice(), m_surface.get())},
          m_resizingMutex{},
          m_imageIndex{UINT32_MAX},
          m_dimensions{ dimensions },
          m_isMinimized{ false }
    {
    }

    Graphics::~Graphics() noexcept
    {
        try
        {
            Clear();
        }
        catch(const std::runtime_error& e)
        {
            NC_LOG_EXCEPTION(e);
        }
    }

    void Graphics::RecreateSwapchain(Vector2 dimensions)
    {
        if (m_isMinimized)
        {
            return;
        }

        std::lock_guard lock{m_resizingMutex};

        // Wait for all current commands to complete execution
        m_device->VkDevice().waitIdle();

        m_dimensions = dimensions;
        m_renderer.reset();
        m_swapchain.reset();

        // Recreate swapchain and resources
        auto shadowMap = ShadowMap { .dimensions = m_dimensions };
        m_shaderResources.get()->GetShadowMapShaderResource().Update(std::vector<ShadowMap>{shadowMap});
        m_swapchain = std::make_unique<Swapchain>(m_device->VkDevice(), m_device->VkPhysicalDevice(), m_surface.get(), m_dimensions);
        m_renderer = std::make_unique<Renderer>(m_device->VkDevice(), m_swapchain.get(), m_gpuOptions.get(), m_allocator.get(), m_shaderResources.get(), dimensions);
    }

    void Graphics::OnResize(float width, float height, WPARAM windowArg)
    {
        m_dimensions = Vector2{ width, height };
        m_isMinimized = windowArg == 1;

        if (m_isMinimized)
        {
            return;
        }

        RecreateSwapchain(m_dimensions);
        InitializeUI();
    }

    void Graphics::Clear()
    {
        m_device->VkDevice().waitIdle();
        m_renderer->Clear();
        ShaderResourceService<ObjectData>::Get()->Reset();
        ShaderResourceService<PointLightInfo>::Get()->Reset();
        ShaderResourceService<ShadowMap>::Get()->Reset();
        ShaderResourceService<EnvironmentData>::Get()->Reset();
    }

    void Graphics::InitializeUI() /** @todo: I hate this whole implementation of ImGui and want to create an abstraction layer for it. */
    {
        m_renderer->InitializeImgui(*m_instance, *m_device, static_cast<uint32_t>(m_gpuOptions->GetMaxSamplesCount()));
    }

    bool Graphics::FrameBegin()
    {
        OPTICK_CATEGORY("Graphics::FrameBegin", Optick::Category::Rendering);
        if (m_isMinimized) return false;

        // Gets the next image in the swapchain
        if(!m_swapchain->GetNextRenderReadyImageIndex(m_frameManager->CurrentFrameContext(), &m_imageIndex))
        {
            RecreateSwapchain(m_dimensions);
        }

        m_frameManager->Begin();

        return true;
    }

    // Executes the command buffer for the next swapchain image which writes to the image.
    // Then, returns the image written to to the swap chain for presentation.
    // Note: All calls below are asynchronous fire-and-forget methods. A maximum of MaxFramesInFlight sets of calls will be running at any given time.
    void Graphics::Draw(const PerFrameRenderState& state)
    {
        OPTICK_CATEGORY("Graphics::Draw", Optick::Category::Rendering);
        auto* currentFrame = m_frameManager->CurrentFrameContext();
        if (m_isMinimized) return;

        m_renderer->Record(currentFrame, state, m_gpuAssetsStorage.get()->meshStorage, m_imageIndex);

        // Executes the command buffer to render to the image
        m_swapchain->WaitForNextImage(currentFrame, m_imageIndex);
        currentFrame->RenderFrame(m_device->VkGraphicsQueue());

        // Returns the image to the swapchain
        bool isSwapChainValid = true;
        m_swapchain->Present(currentFrame, m_device->VkGraphicsQueue(), m_imageIndex, isSwapChainValid);

        if (!isSwapChainValid)
        {
            RecreateSwapchain(m_dimensions);
            return;
        }
    }

    void Graphics::FrameEnd()
    {
        m_frameManager->End();
    }
} // namespace nc::graphics