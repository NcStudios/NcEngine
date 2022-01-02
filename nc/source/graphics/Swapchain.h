#pragma once

#include "math/Vector.h"
#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    class Base; class RenderTarget;

    enum class SemaphoreType : uint8_t
    {
        ImageAvailableForRender,
        PresentReady
    };

    enum class FenceType : uint8_t
    {
        FramesInFlight,
        ImagesInFlight
    };

    class Swapchain
    {
        public:
            Swapchain(Base* base, Vector2 dimensions);
            ~Swapchain() noexcept;

            // Swap chain
            void Present(uint32_t imageIndex, bool& isSwapChainValid);
            void Create(Vector2 dimensions);
            void Cleanup() noexcept;
            const Vector2 GetExtentDimensions() const noexcept;
            const vk::Extent2D& GetExtent() const noexcept;
            const vk::Format& GetFormat() const noexcept;
            const std::vector<vk::ImageView>& GetColorImageViews() const noexcept;
            vk::ImageView& GetDepthView() const noexcept;

            // Image synchronization
            void CreateSynchronizationObjects();
            void DestroySynchronizationObjects() noexcept;
            uint32_t GetNextRenderReadyImageIndex(bool& isSwapChainValid);
            uint32_t GetFrameIndex() const noexcept;
            void WaitForFrameFence() const;
            void ResetFrameFence();
            void IncrementFrameIndex();
            void WaitForImageFence(uint32_t imageIndex);
            void SyncImageAndFrameFence(uint32_t imageIndex);
            const std::vector<vk::Fence>& GetFences(FenceType fenceType) const noexcept;
            const std::vector<vk::Semaphore>& GetSemaphores(SemaphoreType semaphoreType) const noexcept;

        private:
            // External members
            Base* m_base;

            // Internal members
            // Swap chain
            vk::SwapchainKHR m_swapChain;
            std::vector<vk::Image> m_swapChainImages;
            vk::Format m_swapChainImageFormat;
            vk::Extent2D m_swapChainExtent;
            std::vector<vk::ImageView> m_swapChainImageViews;

            // Synchronization
            std::vector<vk::Fence> m_imagesInFlightFences;
            std::vector<vk::Fence> m_framesInFlightFences; // One per concurrent frame. (MAX_FRAMES_IN_FLIGHT). Synchronizes the submission of the queues from the CPU with the completion of the queues on the GPU.
            std::vector<vk::Semaphore> m_imageAvailableSemaphores; // One per concurrent frame. (MAX_FRAMES_IN_FLIGHT). Controls when the swapchain image can be written to.
            std::vector<vk::Semaphore> m_renderFinishedSemaphores; // One per concurrent frame. (MAX_FRAMES_IN_FLIGHT). Controls when the swapchain image can be presented back to the swapchain.
            uint32_t m_currentFrameIndex; // Used to select which pair of semaphores and which fence to use as each frame in MAX_FRAMES_IN_FLIGHT requires its own pair of semaphores and fence.
    };
}