#pragma once

#include "vulkan/vk_mem_alloc.hpp"

#include <vector>

namespace nc::graphics
{
class Base;
class Swapchain;
struct IndexBuffer;
struct VertexBuffer;

class Commands
{
    public:
        Commands(Base* base, const Swapchain& swapchain);
        ~Commands() noexcept;

        std::vector<vk::CommandBuffer>* GetCommandBuffers();
        void FreeCommandBuffers() noexcept;
        void SubmitRenderCommand(uint32_t imageIndex);
        static void SubmitCopyCommandImmediate(const Base& base, const vk::Buffer& sourceBuffer, const vk::Buffer& destinationBuffer, const vk::DeviceSize size);
        static void SubmitCommandImmediate(const Base& base, std::function<void(vk::CommandBuffer cmd)>&& function);
        void BindMeshBuffers(vk::CommandBuffer* cmd, const VertexBuffer& vertexData, const IndexBuffer& indexData);

    private:
        // External members
        Base* m_base;
        const Swapchain& m_swapchain;
        const std::vector<vk::Semaphore>& m_renderReadySemaphores;
        const std::vector<vk::Semaphore>& m_presentReadySemaphores;
        const std::vector<vk::Fence>& m_framesInFlightFences;
        const std::vector<vk::Fence>& m_imagesInFlightFences;

        // Internal members
        std::vector<vk::CommandBuffer> m_commandBuffers;
};
} // namespace nc::graphics