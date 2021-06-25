#include "Graphics2.h"
#include "debug/Profiler.h"
#include "vulkan/Base.h"
#include "vulkan/Commands.h"
#include "vulkan/resources/DepthStencil.h"
#include "vulkan/Swapchain.h"
#include "vulkan/Renderer.h"
#include "vulkan/resources/ResourceManager.h"
#include "config/Config.h"

namespace
{
    constexpr std::array<float, 4> DefaultClearColor = {0.0f, 0.0f, 0.0f, 1.0f};
}

namespace nc::graphics
{
    Graphics2::Graphics2(HWND hwnd, HINSTANCE hinstance, Vector2 dimensions)
        : m_base{ std::make_unique<vulkan::Base>(hwnd, hinstance) },
        m_depthStencil{ std::make_unique<vulkan::DepthStencil>(m_base.get(), dimensions) }, 
        m_swapchain{ std::make_unique<vulkan::Swapchain>(m_base.get(), *m_depthStencil, dimensions) },
        m_commands{ std::make_unique<vulkan::Commands>(m_base.get(), *m_swapchain) },
        m_renderer{ nullptr },
        m_dimensions{ dimensions },
        m_isMinimized{ false },
        m_isFullscreen{ false },
        m_cameraWorldPosition{},
        m_viewMatrix{},
        m_projectionMatrix{},
        m_clearColor{DefaultClearColor},
        m_drawCallCount{0}
    {
        SetProjectionMatrix(dimensions.x, dimensions.y, config::GetGraphicsSettings().nearClip, config::GetGraphicsSettings().farClip);
    }

    Graphics2::~Graphics2()
    {
        vulkan::ResourceManager::Clear();
    }

    void Graphics2::RecreateSwapchain(Vector2 dimensions)
    {
        // Wait for all current commands to complete execution
        WaitIdle();

        // Destroy all resources used by the swapchain
        m_dimensions = dimensions;
        m_depthStencil.reset();
        m_commands.reset();
        m_swapchain.reset();
        m_depthStencil.reset();

        // Recreate swapchain and resources
        m_depthStencil = std::make_unique<vulkan::DepthStencil>(m_base.get(), dimensions);
        m_swapchain = std::make_unique<vulkan::Swapchain>(m_base.get(), *m_depthStencil, dimensions);
        m_commands = std::make_unique<vulkan::Commands>(m_base.get(), *m_swapchain);
    }

    DirectX::FXMMATRIX Graphics2::GetViewMatrix() const noexcept
    {
        return m_viewMatrix;
    }

    DirectX::FXMMATRIX Graphics2::GetProjectionMatrix() const noexcept
    {
        return m_projectionMatrix;
    }

    void Graphics2::SetViewMatrix(DirectX::FXMMATRIX cam) noexcept
    {
        m_viewMatrix = cam;
    }

    void Graphics2::SetCameraPosition(Vector3 cameraPosition)
    {
        m_cameraWorldPosition = cameraPosition;
    }

    const Vector3 Graphics2::GetCameraPosition() const noexcept
    {
        return m_cameraWorldPosition;
    }

    void Graphics2::SetProjectionMatrix(float width, float height, float nearZ, float farZ) noexcept
    {
        m_projectionMatrix = DirectX::XMMatrixPerspectiveRH(1.0f, height / width, nearZ, farZ);
    }

    void Graphics2::ToggleFullscreen()
    {
        // @todo
    }

    void Graphics2::ResizeTarget(float width, float height)
    {
        (void)width;
        (void)height;

        // @todo
    }

    void Graphics2::OnResize(float width, float height, float nearZ, float farZ, WPARAM windowArg)
    {
        (void)width;
        (void)height;
        (void)nearZ;
        (void)farZ;

        m_dimensions = Vector2{ width, height };
        SetProjectionMatrix(width, height, nearZ, farZ);
        m_isMinimized = windowArg == 1;
    }

    void Graphics2::WaitIdle()
    {
        m_base->GetDevice().waitIdle();
    }

    vulkan::Base* Graphics2::GetBasePtr() const noexcept
    {
        return m_base.get();
    }

    const vulkan::Base& Graphics2::GetBase() const noexcept
    {
        return *m_base.get();
    }
    
    vulkan::Swapchain* Graphics2::GetSwapchainPtr() const noexcept
    {
        return m_swapchain.get();
    }

    vulkan::Commands* Graphics2::GetCommandsPtr() const noexcept
    {
        return m_commands.get();
    }

    vulkan::Renderer* Graphics2::GetRendererPtr() const noexcept
    {
        return m_renderer;
    }

    const Vector2 Graphics2::GetDimensions() const noexcept
    {
        return m_dimensions;
    }

    bool Graphics2::GetNextImageIndex(uint32_t* imageIndex)
    {
        m_swapchain->WaitForFrameFence(false);

        bool isSwapChainValid = true;
        *imageIndex = m_swapchain->GetNextRenderReadyImageIndex(isSwapChainValid);
        if (!isSwapChainValid)
        {
            RecreateSwapchain(m_dimensions);
            return false;
        }
        return true;
    }
    
    void Graphics2::SetClearColor(std::array<float, 4> color)
    {
        m_clearColor = color;
    }

    void Graphics2::SetRenderer(vulkan::Renderer* renderer)
    {
        m_renderer = renderer;
    }

    const std::array<float, 4>& Graphics2::GetClearColor() const noexcept
    {
        return m_clearColor;
    }

    void Graphics2::RenderToImage(uint32_t imageIndex)
    {
        m_swapchain->WaitForImageFence(imageIndex);
        m_swapchain->SyncImageAndFrameFence(imageIndex);
        m_swapchain->ResetFrameFence();
        m_commands->SubmitRenderCommand(imageIndex);
    }

    bool Graphics2::PresentImage(uint32_t imageIndex)
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

    void Graphics2::FrameBegin()
    {
        m_drawCallCount = 0;
    }

    // Gets an image from the swap chain, executes the command buffer for that image which writes to the image.
    // Then, returns the image written to to the swap chain for presentation.
    // Note: All calls below are asynchronous fire-and-forget methods. A maximum of Device::MAX_FRAMES_IN_FLIGHT sets of calls will be running at any given time.
    // See Device.cpp for synchronization of these calls.
    void Graphics2::Draw()
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);
        if (m_isMinimized) return;

        uint32_t imageIndex = UINT32_MAX;

        // Gets the next image in the swapchain
        if (!GetNextImageIndex(&imageIndex)) return;

        // Executes the command buffer to render to the image
        RenderToImage(imageIndex);

        // Returns the image to the swapchain
        if (!PresentImage(imageIndex)) return;
        NC_PROFILE_END();
    }

    void Graphics2::FrameEnd()
    {
        // Used to coordinate semaphores and fences because we have multiple concurrent frames being rendered asynchronously
        m_swapchain->IncrementFrameIndex();
    }
    #ifdef NC_EDITOR_ENABLED
    void Graphics2::IncrementDrawCallCount()
    {
        m_drawCallCount++;
    }
    
    uint32_t Graphics2::GetDrawCallCount() const
    {
        return m_drawCallCount;
    }

    #endif
}