#include "Graphics.h"
#include "assets/AssetServices.h"
#include "Base.h"
#include "Commands.h"
#include "config/Config.h"
#include "FrameManager.h"
#include "graphics/Camera.h"
#include "graphics/GpuAssetsStorage.h"
#include "optick/optick.h"
#include "Renderer.h"
#include "resources/ShaderResourceServices.h"
#include "resources/RenderPassManager.h"
#include "utility/Log.h"
#include "utility/NcError.h"
#include "vk/Engine.h"
#include "vk/Swapchain.h"

#include <iostream>

namespace nc::graphics
{
    Graphics::Graphics(camera::MainCamera* mainCamera, const nc::GpuAccessorSignals& gpuAccessorSignals, HWND hwnd, HINSTANCE hinstance, Vector2 dimensions)
        : m_mainCamera{mainCamera},
          m_engine{std::make_unique<Engine>(hwnd, hinstance)},
          m_base{ std::make_unique<Base>(m_engine.get()) },
          m_swapchain{ std::make_unique<Swapchain>(m_engine->logicalDevice.get(), m_engine->physicalDevice, m_engine->surface.get(), dimensions) },
          m_commands{ std::make_unique<Commands>(m_engine->logicalDevice.get(), m_engine->physicalDevice, m_engine->surface.get(), m_swapchain.get()) },
          m_allocator{ std::make_unique<GpuAllocator>(m_engine->logicalDevice.get(), m_engine->physicalDevice, m_engine->instance.get(), m_commands.get())},
          m_shaderResources{ std::make_unique<ShaderResourceServices>(this, m_allocator.get(), config::GetMemorySettings(), dimensions) },
          m_assetServices{ std::make_unique<AssetServices>(config::GetAssetSettings())},
          m_gpuAssetsStorage{ std::make_unique<GpuAssetsStorage>(m_base.get(), m_allocator.get(), gpuAccessorSignals) },
          #ifdef NC_DEBUG_RENDERING_ENABLED
          m_debugRenderer{},
          #endif
          m_renderer{ std::make_unique<Renderer>(this, m_engine->logicalDevice.get(), m_shaderResources.get(), dimensions) },
          m_frameManager{std::make_unique<FrameManager>(m_engine->logicalDevice.get(), m_engine->physicalDevice, m_engine->surface.get())},
          m_resizingMutex{},
          m_imageIndex{UINT32_MAX},
          m_dimensions{ dimensions },
          m_isMinimized{ false },
          m_clearColor{0.0f, 0.0f, 0.0f, 1.0f}
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
        m_engine->logicalDevice.get().waitIdle();

        m_dimensions = dimensions;
        m_renderer.reset();
        m_commands.reset();
        m_swapchain.reset();

        // Recreate swapchain and resources
        auto shadowMap = ShadowMap { .dimensions = m_dimensions };
        m_shaderResources.get()->GetShadowMapManager().Update(std::vector<ShadowMap>{shadowMap});
        m_swapchain = std::make_unique<Swapchain>(m_engine->logicalDevice.get(), m_engine->physicalDevice, m_engine->surface.get(), m_dimensions);
        m_commands = std::make_unique<Commands>(m_engine->logicalDevice.get(), m_engine->physicalDevice, m_engine->surface.get(), m_swapchain.get());
        m_renderer = std::make_unique<Renderer>(this, m_engine->logicalDevice.get(), m_shaderResources.get(), m_dimensions);
    }

    void Graphics::OnResize(float width, float height, float nearZ, float farZ, WPARAM windowArg)
    {
        m_dimensions = Vector2{ width, height };
        m_mainCamera->Get()->UpdateProjectionMatrix(width, height, nearZ, farZ);
        m_isMinimized = windowArg == 1;

        if (m_isMinimized)
        {
            return;
        }

        RecreateSwapchain(m_dimensions);
    }

    Base* Graphics::GetBasePtr() const noexcept
    {
        return m_base.get();
    }

    GpuAllocator* Graphics::GetAllocatorPtr() const noexcept
    {
        return m_allocator.get();
    }

    Swapchain* Graphics::GetSwapchainPtr() const noexcept
    {
        return m_swapchain.get();
    }

    ShaderResourceServices* Graphics::GetShaderResources() const noexcept
    {
        return m_shaderResources.get();
    }

    const Vector2 Graphics::GetDimensions() const noexcept
    {
        return m_dimensions;
    }

    #ifdef NC_DEBUG_RENDERING_ENABLED
    graphics::DebugData* Graphics::GetDebugData()
    {
        return m_debugRenderer.GetData();
    }
    #endif

    void Graphics::Clear()
    {
        m_engine->logicalDevice.get().waitIdle();
        m_renderer->Clear();
        ShaderResourceService<ObjectData>::Get()->Reset();
        ShaderResourceService<PointLightInfo>::Get()->Reset();
        ShaderResourceService<ShadowMap>::Get()->Reset();
        ShaderResourceService<EnvironmentData>::Get()->Reset();
    }

    void Graphics::SetClearColor(std::array<float, 4> color)
    {
        m_clearColor = color;
    }

    const std::array<float, 4>& Graphics::GetClearColor() const noexcept
    {
        return m_clearColor;
    }

    void Graphics::InitializeUI() /** @todo: I hate this whole implementation of ImGui and want to create an abstraction layer for it. */
    {
        m_renderer->InitializeImgui(m_engine->instance.get(), m_engine->physicalDevice, m_engine->logicalDevice.get(), m_commands.get(), static_cast<uint32_t>(m_base->GetMaxSamplesCount()));
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
        if (m_isMinimized) return;

        m_renderer->Record(m_frameManager->CurrentFrameContext(), state, m_gpuAssetsStorage.get()->meshStorage, m_imageIndex);

        // Executes the command buffer to render to the image
        m_swapchain->WaitForImageFence(m_imageIndex);
        m_swapchain->SyncImageAndFrameFence(m_frameManager->CurrentFrameContext(), m_imageIndex);
        m_commands->SubmitQueue(m_frameManager->CurrentFrameContext());

        // Returns the image to the swapchain
        bool isSwapChainValid = true;
        m_swapchain->Present(m_frameManager->CurrentFrameContext(), m_commands->GetCommandQueue(QueueFamilyType::GraphicsFamily), m_imageIndex, isSwapChainValid);

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