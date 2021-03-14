#include "Commands.h"
#include "Base.h"
#include "GraphicsPipeline.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Swapchain.h"

namespace nc::graphics::vulkan
{
    Commands::Commands(const vulkan::Base& base, const vulkan::Swapchain& swapchain)
    : m_base{ base },
      m_swapchain{ swapchain },
      m_renderReadySemaphores{ m_swapchain.GetSemaphores(SemaphoreType::RenderReady) },
      m_presentReadySemaphores{ m_swapchain.GetSemaphores(SemaphoreType::PresentReady)  },
      m_framesInFlightFences{ m_swapchain.GetFences(FenceType::FramesInFlight) },
      m_imagesInFlightFences{ m_swapchain.GetFences(FenceType::ImagesInFlight) },
      m_commandBuffers{}
    {
        // Create the command buffers.
        m_commandBuffers.resize(m_swapchain.GetSwapChainImageViews().size()); // Need to have one command buffer per frame buffer, which have the same count as the image views.
        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.setCommandPool(m_base.GetCommandPool());
        allocInfo.setLevel(vk::CommandBufferLevel::ePrimary); // Primary means the command buffer can be submitted to a queue for execution, but not called from other command buffers. Alternative is Secondary, which cant be submitted directly but can be called from other primary command buffers.
        allocInfo.setCommandBufferCount(static_cast<uint32_t>(m_commandBuffers.size()));
        m_commandBuffers = m_base.GetDevice().allocateCommandBuffers(allocInfo);
    }

    void Commands::RecordRenderCommand(const vulkan::GraphicsPipeline& pipeline, const vulkan::Swapchain& swapchain, const vulkan::VertexBuffer& vertexBuffer, const vulkan::IndexBuffer& indexBuffer)
    {
        const vk::ClearValue clearValues[1] = { vk::ClearColorValue(std::array<float, 4>({{0.2f, 0.2f, 0.2f, 0.2f}})) };

        // Begin recording on each of the command buffers.
        for (size_t i = 0; i < m_commandBuffers.size(); ++i)
        {
            vk::CommandBufferBeginInfo beginInfo;
            beginInfo.setPInheritanceInfo(nullptr);

            // Begin recording commands to each command buffer.
            m_commandBuffers[i].begin(beginInfo);
            {
                vk::RenderPassBeginInfo renderPassInfo{};
                renderPassInfo.setRenderPass(m_swapchain.GetFrameBufferFillPass()); // Specify the render pass and attachments.
                renderPassInfo.setFramebuffer(m_swapchain.GetFrameBuffer((uint32_t)i));
                renderPassInfo.renderArea.setOffset({0,0}); // Specify the dimensions of the render area.
                renderPassInfo.renderArea.setExtent(m_swapchain.GetSwapChainExtent());
                renderPassInfo.setClearValueCount(1); // Set clear color
                renderPassInfo.setPClearValues(clearValues);

                m_commandBuffers[i].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
                {
                    m_commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.GetPipeline());

                    // Viewport defines the transformation from the image to the framebuffer
                    auto [width, height] = swapchain.GetSwapChainExtentDimensions();
                    vk::Viewport viewport{};
                    viewport.setX(0.0f);
                    viewport.setY(0.0f);
                    viewport.setWidth(width);
                    viewport.setHeight(height);
                    viewport.setMinDepth(0.0f);
                    viewport.setMaxDepth(1.0f);

                    // Scissor rectangles crop the image, discarding pixels outside of the scissor rect by the rasterizer.
                    vk::Rect2D scissor{};
                    scissor.setOffset({0,0});
                    scissor.setExtent(swapchain.GetSwapChainExtent());

                    m_commandBuffers[i].setViewport(0, viewport);
                    m_commandBuffers[i].setScissor(0, scissor);

                    vk::Buffer vertexBuffers[] = { vertexBuffer.GetBuffer() };
                    vk::DeviceSize offsets[] = { 0 };
                    m_commandBuffers[i].bindVertexBuffers(0, 1, vertexBuffers, offsets);
                    m_commandBuffers[i].bindIndexBuffer(indexBuffer.GetBuffer(), 0, vk::IndexType::eUint32);
                    m_commandBuffers[i].drawIndexed(static_cast<uint32_t>(indexBuffer.GetIndices().size()), 1, 0, 0, 0); // indexCount, instanceCount, firstIndex, vertexOffset, firstInstance
                }
                m_commandBuffers[i].endRenderPass();
            }
            m_commandBuffers[i].end();
        }
    }

    void Commands::SubmitRenderCommand(uint32_t imageIndex)
    {
        auto currentFrameIndex = m_swapchain.GetFrameIndex();

        vk::Semaphore waitSemaphores[] = { m_renderReadySemaphores[currentFrameIndex] }; // Which semaphore to wait on before execution begins
        vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput }; // Which stage of the pipeline to wait in
        vk::Semaphore signalSemaphores[] = { m_presentReadySemaphores[currentFrameIndex] }; // Which semaphore to signal when execution completes

        vk::SubmitInfo submitInfo{};
        submitInfo.setWaitSemaphoreCount(1);
        submitInfo.setPWaitSemaphores(waitSemaphores);
        submitInfo.setPWaitDstStageMask(waitStages);
        submitInfo.setCommandBufferCount(1);
        submitInfo.setPCommandBuffers(&m_commandBuffers[imageIndex]);
        submitInfo.setSignalSemaphoreCount(1);
        submitInfo.setPSignalSemaphores(signalSemaphores);

        m_base.GetQueue(QueueFamilyType::GraphicsFamily).submit(submitInfo, m_framesInFlightFences[currentFrameIndex]);
    }

    void Commands::SubmitCopyCommandImmediate(const vk::Buffer& sourceBuffer, const vk::Buffer& destinationBuffer, const vk::DeviceSize size)
    {
        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
        allocInfo.setCommandPool(m_base.GetCommandPool());
        allocInfo.setCommandBufferCount(1);

        auto tempCommandBuffers = m_base.GetDevice().allocateCommandBuffers(allocInfo);
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
        m_base.GetQueue(QueueFamilyType::GraphicsFamily).submit(submitInfo, nullptr);
        m_base.GetQueue(QueueFamilyType::GraphicsFamily).waitIdle();
        m_base.GetDevice().freeCommandBuffers(m_base.GetCommandPool(), tempCommandBuffer);
    }
}