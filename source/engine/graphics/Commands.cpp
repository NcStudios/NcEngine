#include "Commands.h"
#include "Base.h"
#include "vk/Swapchain.h"
#include "vk/Meshes.h"

namespace nc::graphics
{
    Commands::Commands(Base* base, Swapchain* swapchain)
    : m_base{ base },
      m_swapchain{ swapchain },
        m_commandBuffers{} 
    {
        // Create the command buffers.
        m_commandBuffers.resize(m_swapchain->GetColorImageViews().size()); // Need to have one command buffer per frame buffer, which have the same count as the image views.
        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.setCommandPool(m_base->GetCommandPool());
        allocInfo.setLevel(vk::CommandBufferLevel::ePrimary); // Primary means the command buffer can be submitted to a queue for execution, but not called from other command buffers. Alternative is Secondary, which cant be submitted directly but can be called from other primary command buffers.
        allocInfo.setCommandBufferCount(static_cast<uint32_t>(m_commandBuffers.size()));
        m_commandBuffers = m_base->GetDevice().allocateCommandBuffers(allocInfo);
    }

    Commands::~Commands()
    {
        FreeCommandBuffers();
    }

    std::vector<vk::CommandBuffer>* Commands::GetCommandBuffers()
    {
        return &m_commandBuffers;
    }

    void Commands::FreeCommandBuffers() noexcept
    {
        m_base->FreeCommandBuffers(&m_commandBuffers);
    }

    void Commands::SubmitRenderCommand(uint32_t imageIndex)
    {
        auto currentFrameIndex = m_swapchain->GetFrameIndex();

        vk::Semaphore waitSemaphores[] = { m_swapchain->GetPerFrameGpuContext().ImageAvailableSemaphore() }; // Which semaphore to wait on before execution begins
        vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput }; // Which stage of the pipeline to wait in
        vk::Semaphore signalSemaphores[] = { m_swapchain->GetPerFrameGpuContext().RenderFinishedSemaphore() }; // Which semaphore to signal when execution completes

        vk::SubmitInfo submitInfo{};
        submitInfo.setWaitSemaphoreCount(1);
        submitInfo.setPWaitSemaphores(waitSemaphores);
        submitInfo.setPWaitDstStageMask(waitStages);
        submitInfo.setCommandBufferCount(1);
        submitInfo.setPCommandBuffers(&m_commandBuffers[imageIndex]);
        submitInfo.setSignalSemaphoreCount(1);
        submitInfo.setPSignalSemaphores(signalSemaphores);

        m_base->GetDevice().resetFences(m_swapchain->GetPerFrameGpuContext().Fence());
        m_base->GetQueue(QueueFamilyType::GraphicsFamily).submit(submitInfo, m_swapchain->GetPerFrameGpuContext().Fence());
    }

    void Commands::SubmitCopyCommandImmediate(const Base& base, const vk::Buffer& sourceBuffer, const vk::Buffer& destinationBuffer, const vk::DeviceSize size)
    {
        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
        allocInfo.setCommandPool(base.GetCommandPool());
        allocInfo.setCommandBufferCount(1);

        auto tempCommandBuffers = base.GetDevice().allocateCommandBuffers(allocInfo);
        auto tempCommandBuffer = tempCommandBuffers[0];

        // Begin recording immediately
        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        tempCommandBuffer.begin(beginInfo);
        {
            vk::BufferCopy copyRegion{};
            copyRegion.setSize(size);
            tempCommandBuffer.copyBuffer(sourceBuffer, destinationBuffer, 1, &copyRegion);
        }
        tempCommandBuffer.end();

        // Submit the command immediately
        vk::SubmitInfo submitInfo{};
        submitInfo.setCommandBufferCount(1);
        submitInfo.setPCommandBuffers(&tempCommandBuffer);
        base.GetQueue(QueueFamilyType::GraphicsFamily).submit(submitInfo, nullptr);
        base.GetQueue(QueueFamilyType::GraphicsFamily).waitIdle();
        base.GetDevice().freeCommandBuffers(base.GetCommandPool(), tempCommandBuffer);
    }

    void Commands::SubmitCommandImmediate(const Base& base, std::function<void(vk::CommandBuffer cmd)>&& function)
    {
        auto allocInfo = vk::CommandBufferAllocateInfo{};
        allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
        allocInfo.setCommandPool(base.GetCommandPool());
        allocInfo.setCommandBufferCount(1);

        auto tempCommandBuffers = base.GetDevice().allocateCommandBuffers(allocInfo);
        auto tempCommandBuffer = tempCommandBuffers[0];

        auto beginInfo = vk::CommandBufferBeginInfo{};
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        tempCommandBuffer.begin(beginInfo);
        {
            function(tempCommandBuffer);
        }
        tempCommandBuffer.end();

        auto submitInfo = vk::SubmitInfo{};
        submitInfo.setCommandBufferCount(1);
        submitInfo.setPCommandBuffers(&tempCommandBuffer);
        base.GetQueue(QueueFamilyType::GraphicsFamily).submit(submitInfo, nullptr);
        base.GetQueue(QueueFamilyType::GraphicsFamily).waitIdle();
        base.GetDevice().freeCommandBuffers(base.GetCommandPool(), tempCommandBuffer);
    }

    void Commands::BindMeshBuffers(vk::CommandBuffer* cmd, const VertexBuffer& vertexData, const IndexBuffer& indexData)
    {
        vk::DeviceSize offsets[] = { 0 };
        auto vertexBuffer = vertexData.buffer.GetBuffer();
        cmd->bindVertexBuffers(0, 1, &vertexBuffer, offsets);
        cmd->bindIndexBuffer(indexData.buffer.GetBuffer(), 0, vk::IndexType::eUint32);
    }
}  // namespace nc::graphics