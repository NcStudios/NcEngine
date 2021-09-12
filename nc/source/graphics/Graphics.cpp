#include "Graphics.h"
#include "camera/MainCameraInternal.h"
#include "debug/Profiler.h"
#include "debug/Utils.h"
#include "Base.h"
#include "Commands.h"
#include "resources/DepthStencil.h"
#include "Swapchain.h"
#include "Renderer.h"
#include "resources/ResourceManager.h"
#include "config/Config.h"

namespace
{
    constexpr std::array<float, 4> DefaultClearColor = {0.0f, 0.0f, 0.0f, 1.0f};
}

namespace nc::graphics
{
    Graphics::Graphics(HWND hwnd, HINSTANCE hinstance, Vector2 dimensions)
        : m_base{ std::make_unique<Base>(hwnd, hinstance) },
          m_depthStencil{ std::make_unique<DepthStencil>(m_base.get(), dimensions) }, 
          m_swapchain{ std::make_unique<Swapchain>(m_base.get(), *m_depthStencil, dimensions) },
          m_commands{ std::make_unique<Commands>(m_base.get(), *m_swapchain) },
          m_renderer{ nullptr },
          m_resourceManager{std::make_unique<ResourceManager>()},
          m_imageIndex{UINT32_MAX},
          m_dimensions{ dimensions },
          m_isMinimized{ false },
          m_isFullscreen{ false },
          m_clearColor{DefaultClearColor},
          m_drawCallCount{0}
    {
        camera::UpdateProjectionMatrix(dimensions.x, dimensions.y, config::GetGraphicsSettings().nearClip, config::GetGraphicsSettings().farClip);
    }

    Graphics::~Graphics() noexcept
    {
        try
        {
            Clear();
        }
        catch(const std::runtime_error& e) // from WaitIdle()
        {
            debug::LogException(e);
        }
    }

    void Graphics::RecreateSwapchain(Vector2 dimensions)
    {
        // Wait for all current commands to complete execution
        WaitIdle();

        // Destroy all resources used by the swapchain
        m_dimensions = dimensions;
        m_commands.reset();
        m_swapchain.reset();
        m_depthStencil.reset();

        ResourceManager::ResizeShadowMap(dimensions);
        m_renderer->InitializeShadowMappingRenderPass();

        // Recreate swapchain and resources
        m_depthStencil = std::make_unique<DepthStencil>(m_base.get(), dimensions);
        m_swapchain = std::make_unique<Swapchain>(m_base.get(), *m_depthStencil, dimensions);
        m_commands = std::make_unique<Commands>(m_base.get(), *m_swapchain);
    }

    void Graphics::ToggleFullscreen()
    {
        // @todo
    }

    void Graphics::ResizeTarget(float width, float height)
    {
        (void)width;
        (void)height;

        // @todo
    }

    void Graphics::OnResize(float width, float height, float nearZ, float farZ, WPARAM windowArg)
    {
        (void)width;
        (void)height;
        (void)nearZ;
        (void)farZ;

        m_dimensions = Vector2{ width, height };
        camera::UpdateProjectionMatrix(width, height, nearZ, farZ);
        m_isMinimized = windowArg == 1;
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

    Renderer* Graphics::GetRendererPtr() const noexcept
    {
        return m_renderer;
    }

    const Vector2 Graphics::GetDimensions() const noexcept
    {
        return m_dimensions;
    }

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
    
    void Graphics::Clear()
    {
        WaitIdle();
        m_renderer->Clear();
        ResourceManager::Clear();
    }
    
    void Graphics::SetClearColor(std::array<float, 4> color)
    {
        m_clearColor = color;
    }

    void Graphics::SetRenderer(Renderer* renderer)
    {
        m_renderer = renderer;
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

    uint32_t Graphics::FrameBegin()
    {
        m_drawCallCount = 0;
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);
        uint32_t imageIndex = UINT32_MAX;

        // Gets the next image in the swapchain
        GetNextImageIndex(&imageIndex);

        return m_imageIndex;
    }

    // Executes the command buffer for the next swapchain image which writes to the image.
    // Then, returns the image written to to the swap chain for presentation.
    // Note: All calls below are asynchronous fire-and-forget methods. A maximum of Device::MAX_FRAMES_IN_FLIGHT sets of calls will be running at any given time.
    // See Device.cpp for synchronization of these calls.
    void Graphics::Draw()
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);
        if (m_isMinimized) return;

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

    #ifdef NC_EDITOR_ENABLED
    void Graphics::IncrementDrawCallCount()
    {
        m_drawCallCount++;
    }
    
    uint32_t Graphics::GetDrawCallCount() const
    {
        return m_drawCallCount;
    }

    #endif
}