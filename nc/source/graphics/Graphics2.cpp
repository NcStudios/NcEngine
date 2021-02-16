#include "Graphics2.h"
#include "vulkan/Device.h"
#include "vulkan/Instance.h"
#include "vulkan/GraphicsPipeline.h"
#include "vulkan/RenderPass.h"
#include "vulkan/FrameBuffers.h"
#include "vulkan/Commands.h"

namespace nc::graphics
{
    using namespace vulkan;

    Graphics2::Graphics2(HWND hwnd, HINSTANCE hinstance, Vector2 dimensions)
        : m_instance{ nullptr },
        m_device{ nullptr },
        m_pipeline{ nullptr },
        m_renderPass{ nullptr },
        m_frameBuffers{ nullptr },
        m_commands{ nullptr },
        m_dimensions{ dimensions },
        m_isMinimized{ false },
        m_isFullscreen{ false },
        m_viewMatrix{},
        m_projectionMatrix{}
    {
        m_instance = std::make_unique<Instance>(hwnd, hinstance);
        m_device = std::make_unique<Device>(m_instance.get(), dimensions);
        m_renderPass = std::make_unique<RenderPass>(m_device.get());
        m_pipeline = std::make_unique<GraphicsPipeline>(m_device.get(), m_renderPass.get());
        m_frameBuffers = std::make_unique<FrameBuffers>(m_device.get(), m_renderPass.get());
        m_commands = std::make_unique<Commands>(m_device.get(),
            m_device->GetSemaphores(SemaphoreType::RenderReady),
            m_device->GetSemaphores(SemaphoreType::PresentReady),
            m_device->GetFences(FenceType::FramesInFlight),
            m_device->GetFences(FenceType::ImagesInFlight));

        // Write the render pass to the command buffer.
        m_commands->RecordRenderPass(m_device.get(), m_renderPass.get(), m_frameBuffers.get(), m_pipeline.get());
    }

    Graphics2::~Graphics2() = default;

    void Graphics2::RecreateSwapChain(Vector2 dimensions)
    {
        // Wait for all current commands to complete execution
        WaitIdle();

        // Destroy all resources used by the swapchain
        m_commands.reset();
        m_frameBuffers.reset();
        m_pipeline.reset(); // @todo: Set up dynamic state in pipeline for scissor rect and viewport so entire pipeline is not recreated on resize.
        m_renderPass.reset();
        m_device->CleanupSwapChain();

        // Recreate swapchain and resources
        m_device->CreateSwapChain(dimensions);
        m_renderPass = std::make_unique<RenderPass>(m_device.get());
        m_pipeline = std::make_unique<GraphicsPipeline>(m_device.get(), m_renderPass.get());
        m_frameBuffers = std::make_unique<FrameBuffers>(m_device.get(), m_renderPass.get());
        m_commands = std::make_unique<Commands>(m_device.get(),
            m_device->GetSemaphores(SemaphoreType::RenderReady),
            m_device->GetSemaphores(SemaphoreType::PresentReady),
            m_device->GetFences(FenceType::FramesInFlight),
            m_device->GetFences(FenceType::ImagesInFlight));
        m_commands->RecordRenderPass(m_device.get(), m_renderPass.get(), m_frameBuffers.get(), m_pipeline.get());
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

    void Graphics2::SetProjectionMatrix(float width, float height, float nearZ, float farZ) noexcept
    {
        m_projectionMatrix = DirectX::XMMatrixPerspectiveLH(1.0f, height / width, nearZ, farZ);
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
        m_isMinimized = windowArg == 1;
    }

    void Graphics2::WaitIdle()
    {
        m_device->GetDevice()->waitIdle();
    }

    void Graphics2::FrameBegin()
    {
        // @todo
    }

    bool Graphics2::GetNextImageIndex(uint32_t& imageIndex)
    {
        m_device->WaitForFrameFence();

        bool isSwapChainValid = true;
        imageIndex = m_device->GetNextRenderReadyImageIndex(isSwapChainValid);
        if (!isSwapChainValid)
        {
            RecreateSwapChain(m_dimensions);
            return false;
        }
        return true;
    }

    void Graphics2::RenderToImage(uint32_t imageIndex)
    {
        m_device->WaitForImageFence(imageIndex);
        m_device->SyncImageAndFrameFence(imageIndex);
        m_device->ResetFrameFence();
        m_commands->SubmitCommandBuffer(m_device.get(), imageIndex);
    }

    bool Graphics2::PresentImage(uint32_t imageIndex)
    {
        bool isSwapChainValid = true;
        m_device->Present(imageIndex, isSwapChainValid);

        if (!isSwapChainValid)
        {
            RecreateSwapChain(m_dimensions);
            return false;
        }
        return true;
    }

    // Gets an image from the swap chain, executes the command buffer for that image which writes to the image.
    // Then, returns the image written to to the swap chain for presentation.
    // Note: All calls below are asynchronous fire-and-forget methods. A maximum of Device::MAX_FRAMES_IN_FLIGHT sets of calls will be running at any given time.
    // See Device.cpp for synchronization of these calls.
    void Graphics2::Draw()
    {
        if (m_isMinimized) return;

        uint32_t imageIndex = UINT32_MAX;

        // Gets the next image in the swapchain
        if (!GetNextImageIndex(imageIndex)) return;

        // Executes the command buffer to render to the image
        RenderToImage(imageIndex);

        // Returns the image to the swapchain
        if (!PresentImage(imageIndex)) return;
    }

    void Graphics2::FrameEnd()
    {
        // Used to coordinate semaphores and fences because we have multiple concurrent frames being rendered asynchronously
        m_device->IncrementFrameIndex();
    }
}