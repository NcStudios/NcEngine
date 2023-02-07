#include "Graphics.h"
#include "assets/AssetServices.h"
#include "Commands.h"
#include "config/Config.h"
#include "ecs/Registry.h"
#include "FrameManager.h"
#include "GpuOptions.h"
#include "graphics/Camera.h"
#include "graphics/GpuAssetsStorage.h"
#include "graphics/Imgui.h"
#include "graphics/Lighting.h"
#include "graphics/RenderGraph.h"
#include "optick/optick.h"
#include "shaders/ShaderDescriptorSets.h"
#include "shaders/ShaderResources.h"
#include "utility/Log.h"
#include "ncutility/NcError.h"
#include "Core.h"
#include "Swapchain.h"

#include <iostream>

namespace nc::graphics
{
    Graphics::Graphics(camera::MainCamera* mainCamera, Registry* registry, const nc::GpuAccessorSignals& gpuAccessorSignals, HWND hwnd, HINSTANCE hinstance, Vector2 dimensions)
        : m_mainCamera{mainCamera},
          m_registry{registry},
          m_core{std::make_unique<Core>(hwnd, hinstance)},
          m_gpuOptions{ std::make_unique<GpuOptions>(m_core->physicalDevice) },
          m_swapchain{ std::make_unique<Swapchain>(m_core->logicalDevice.get(), m_core->physicalDevice, m_core->surface.get(), dimensions) },
          m_commands{ std::make_unique<Commands>(m_core->logicalDevice.get(), m_core->physicalDevice, m_core->surface.get(), m_swapchain.get()) },
          m_allocator{ std::make_unique<GpuAllocator>(m_core->logicalDevice.get(), m_core->physicalDevice, m_core->instance.get(), m_commands.get())},
          m_shaderDescriptorSets{ std::make_unique<ShaderDescriptorSets>(m_core->logicalDevice.get())},
          m_shaderResources{ std::make_unique<ShaderResources>(m_core->logicalDevice.get(), m_shaderDescriptorSets.get(), registry, m_allocator.get(), config::GetMemorySettings(), dimensions)},
          m_assetServices{ std::make_unique<AssetServices>(config::GetAssetSettings())},
          m_gpuAssetsStorage{ std::make_unique<GpuAssetsStorage>(m_core->logicalDevice.get(), m_allocator.get(), gpuAccessorSignals) },
          m_renderGraph{std::make_unique<RenderGraph>(m_core->logicalDevice.get(), m_swapchain.get(), m_gpuOptions.get(), m_allocator.get(), m_shaderDescriptorSets.get(), dimensions)},
          m_imgui{ std::make_unique<Imgui>(m_core->logicalDevice.get())},
          m_frameManager{std::make_unique<FrameManager>(m_core->logicalDevice.get(), m_core->physicalDevice, m_core->surface.get())},
          m_lighting{std::make_unique<Lighting>(registry, m_core->logicalDevice.get(), m_allocator.get(), m_gpuOptions.get(), m_swapchain.get(), m_renderGraph.get(), m_shaderDescriptorSets.get(), m_shaderResources.get(), dimensions)},
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

    void Graphics::Resize(const Vector2& dimensions)
    {
        if (m_isMinimized)
        {
            return;
        }

        std::lock_guard lock{m_resizingMutex};

        // Wait for all current commands to complete execution
        m_core->logicalDevice.get().waitIdle();

        m_swapchain->Resize(dimensions);
        m_renderGraph.reset();
        m_renderGraph = std::make_unique<RenderGraph>(m_core->logicalDevice.get(), m_swapchain.get(), m_gpuOptions.get(), m_allocator.get(), m_shaderDescriptorSets.get(), m_dimensions);
        m_lighting.reset();
        m_lighting = std::make_unique<Lighting>(m_registry, m_core->logicalDevice.get(), m_allocator.get(), m_gpuOptions.get(), m_swapchain.get(), m_renderGraph.get(), m_shaderDescriptorSets.get(), m_shaderResources.get(), m_dimensions);
    }

    void Graphics::OnResize(float width, float height, float nearZ, float farZ, WPARAM windowArg)
    {
        m_dimensions = Vector2{ width, height };
        m_mainCamera->Get()->UpdateProjectionMatrix(width, height, nearZ, farZ);
        m_isMinimized = windowArg == 1;
        Resize(m_dimensions);
        InitializeUI();
    }

    void Graphics::Clear()
    {
        m_core->logicalDevice.get().waitIdle();
        m_lighting->Clear();
        ShaderResourceService<ObjectData>::Get()->Reset();
        ShaderResourceService<PointLightInfo>::Get()->Reset();
        ShaderResourceService<ShadowMap>::Get()->Reset();
        ShaderResourceService<EnvironmentData>::Get()->Reset();
    }

    void Graphics::InitializeUI() /** @todo: I hate this whole implementation of ImGui and want to create an abstraction layer for it. */
    {
        m_imgui->InitializeImgui(m_core->instance.get(), m_core->physicalDevice, m_core->logicalDevice.get(), (m_renderGraph->Acquire(LitPassId))->renderPass.get(), m_commands.get(), static_cast<uint32_t>(m_gpuOptions->GetMaxSamplesCount()));
    }

    bool Graphics::FrameBegin()
    {
        OPTICK_CATEGORY("Graphics::FrameBegin", Optick::Category::Rendering);
        if (m_isMinimized) return false;

        // Gets the next image in the swapchain
        if(!m_swapchain->GetNextRenderReadyImageIndex(m_frameManager->CurrentFrameContext(), &m_imageIndex))
        {
            Resize(m_dimensions);
            // return false;
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

        m_renderGraph->Execute(currentFrame, state, m_gpuAssetsStorage.get()->meshStorage, m_imageIndex, m_dimensions);

        // Executes the command buffer to render to the image
        m_swapchain->WaitForNextImage(currentFrame, m_imageIndex);
        currentFrame->RenderFrame(m_commands->GetCommandQueue(QueueFamilyType::GraphicsFamily));

        // Returns the image to the swapchain
        bool isSwapChainValid = true;
        m_swapchain->Present(currentFrame, m_commands->GetCommandQueue(QueueFamilyType::GraphicsFamily), m_imageIndex, isSwapChainValid);

        if (!isSwapChainValid)
        {
            Resize(m_dimensions);
        }
    }

    void Graphics::FrameEnd()
    {
        m_frameManager->End();
    }
} // namespace nc::graphics