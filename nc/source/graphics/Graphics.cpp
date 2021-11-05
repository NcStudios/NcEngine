#include "Graphics.h"
#include "assets/AssetServices.h"
#include "Base.h"
#include "Commands.h"
#include "config/Config.h"
#include "debug/Profiler.h"
#include "debug/Utils.h"
#include "Renderer.h"
#include "resources/DepthStencil.h"
#include "resources/ShaderResourceServices.h"
#include "resources/RenderPassManager.h"
#include "Swapchain.h"

#include <iostream>

namespace
{
    constexpr std::array<float, 4> DefaultClearColor = {0.0f, 0.0f, 0.0f, 1.0f};
}

namespace nc::graphics
{
    Graphics::Graphics(MainCamera* mainCamera, HWND hwnd, HINSTANCE hinstance, Vector2 dimensions)
        : m_mainCamera{mainCamera},
          m_base{ std::make_unique<Base>(hwnd, hinstance) },
          m_depthStencil{ std::make_unique<DepthStencil>(m_base.get(), dimensions) }, 
          m_swapchain{ std::make_unique<Swapchain>(m_base.get(), dimensions) },
          m_commands{ std::make_unique<Commands>(m_base.get(), *m_swapchain) },
          m_shaderResources{ std::make_unique<ShaderResourceServices>(this, config::GetMemorySettings(), dimensions) },
          m_assetServices{ std::make_unique<AssetServices>(this, config::GetMemorySettings().maxTextures) },
          #ifdef NC_DEBUG_RENDERING
          m_debugRenderer{},
          #endif
          m_renderer{ std::make_unique<Renderer>(this, m_shaderResources.get(), dimensions) },
          m_resizingMutex{},
          m_imageIndex{UINT32_MAX},
          m_dimensions{ dimensions },
          m_isMinimized{ false },
          m_clearColor{DefaultClearColor}
    {
    }

    Graphics::~Graphics() noexcept
    {
        try
        {
            WaitIdle();
            Clear();
        }
        catch(const std::runtime_error& e) // from WaitIdle()
        {
            debug::LogException(e);
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
        WaitIdle();

        m_dimensions = dimensions;
        m_renderer.reset();
        m_commands.reset();
        m_swapchain.reset();
        m_depthStencil.reset();

        // Recreate swapchain and resources
        auto shadowMap = ShadowMap { .dimensions = m_dimensions };
        m_shaderResources.get()->GetShadowMapManager().Update(std::vector<ShadowMap>{shadowMap});
        m_depthStencil = std::make_unique<DepthStencil>(m_base.get(), m_dimensions);
        m_swapchain = std::make_unique<Swapchain>(m_base.get(), m_dimensions);
        m_commands = std::make_unique<Commands>(m_base.get(), *m_swapchain);
        m_renderer = std::make_unique<Renderer>(this, m_shaderResources.get(), m_dimensions);
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

    void Graphics::WaitIdle()
    {
        m_base->GetDevice().waitIdle();
    }

    Base* Graphics::GetBasePtr() const noexcept
    {
        return m_base.get();
    }
    
    Swapchain* Graphics::GetSwapchainPtr() const noexcept
    {
        return m_swapchain.get();
    }

    Commands* Graphics::GetCommandsPtr() const noexcept
    {
        return m_commands.get();
    }

    const Vector2 Graphics::GetDimensions() const noexcept
    {
        return m_dimensions;
    }

    #ifdef NC_DEBUG_RENDERING
    graphics::DebugData* Graphics::GetDebugData()
    {
        return m_debugRenderer.GetData();
    }

    void Graphics::ClearDebugRenderer()
    {
        m_debugRenderer.ClearLines();
        m_debugRenderer.ClearPoints();
        m_debugRenderer.ClearPlanes();
    }
    #endif

    bool Graphics::GetNextImageIndex(uint32_t* imageIndex)
    {
        bool isSwapChainValid = true;
        *imageIndex = m_swapchain->GetNextRenderReadyImageIndex(isSwapChainValid);
        m_imageIndex = *imageIndex;
        if (!isSwapChainValid)
        {
            RecreateSwapchain(m_dimensions);
            return false;
        }
        return true;
    }
    
    const DepthStencil& Graphics::GetDepthStencil() const noexcept
    {
        return *(m_depthStencil.get());
    }

    void Graphics::Clear()
    {
        WaitIdle();
        m_renderer->Clear();
        ShaderResourceService<ObjectData>::Get()->Reset();
        ShaderResourceService<PointLightInfo>::Get()->Reset();
        ShaderResourceService<ShadowMap>::Get()->Reset();
    }

    void Graphics::SetClearColor(std::array<float, 4> color)
    {
        m_clearColor = color;
    }

    const std::array<float, 4>& Graphics::GetClearColor() const noexcept
    {
        return m_clearColor;
    }

    void Graphics::RenderToImage(uint32_t imageIndex)
    {
        m_swapchain->WaitForImageFence(imageIndex);
        m_swapchain->SyncImageAndFrameFence(imageIndex);
        m_commands->SubmitRenderCommand(imageIndex);
    }

    bool Graphics::PresentImage(uint32_t imageIndex)
    {
        bool isSwapChainValid = true;
        m_swapchain->Present(imageIndex, isSwapChainValid);

        if (!isSwapChainValid)
        {
            RecreateSwapchain(m_dimensions);
            return false;
        }
        return true;
    }

    void Graphics::InitializeUI()
    {
        m_renderer->InitializeImgui();
    }

    uint32_t Graphics::FrameBegin()
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);
        if (m_isMinimized) return UINT32_MAX;

        uint32_t imageIndex = UINT32_MAX;

        // Gets the next image in the swapchain
        GetNextImageIndex(&imageIndex);

        return m_imageIndex;
    }

    // Executes the command buffer for the next swapchain image which writes to the image.
    // Then, returns the image written to to the swap chain for presentation.
    // Note: All calls below are asynchronous fire-and-forget methods. A maximum of Device::MAX_FRAMES_IN_FLIGHT sets of calls will be running at any given time.
    // See Device.cpp for synchronization of these calls.
    void Graphics::Draw(PerFrameRenderState* state)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);
        if (m_isMinimized) return;

        m_renderer->Record(m_commands.get(), state, m_assetServices.get(), m_imageIndex);

        // Executes the command buffer to render to the image
        RenderToImage(m_imageIndex);

        // Returns the image to the swapchain
        if (!PresentImage(m_imageIndex)) return;

        NC_PROFILE_END();
    }

    void Graphics::FrameEnd()
    {
        // Used to coordinate semaphores and fences because we have multiple concurrent frames being rendered asynchronously
        m_swapchain->IncrementFrameIndex();
    }
}