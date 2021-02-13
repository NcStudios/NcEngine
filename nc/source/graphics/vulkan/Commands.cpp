#include "Commands.h"
#include "Device.h"
#include "RenderPass.h"
#include "FrameBuffers.h"
#include "GraphicsPipeline.h"

namespace nc::graphics::vulkan
{
    Commands::Commands(const vulkan::Device* device, const std::vector<vk::Semaphore>* renderReadySemaphores, const std::vector<vk::Semaphore>* presentReadySemaphores, const std::vector<vk::Fence>* framesInFlightFences, const std::vector<vk::Fence>* imagesInFlightFences)
    : m_commandBuffers{},
      m_renderReadySemaphores{renderReadySemaphores},
      m_presentReadySemaphores{presentReadySemaphores},
      m_framesInFlightFences{framesInFlightFences},
      m_imagesInFlightFences{imagesInFlightFences}
    {
        // Create the command buffers.
        m_commandBuffers.resize(device->GetSwapChainImageViews()->size()); // Need to have one command buffer per frame buffer, which have the same count as the image views.
        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.setCommandPool(*(device->GetCommandPool()));
        allocInfo.setLevel(vk::CommandBufferLevel::ePrimary); // Primary means the command buffer can be submitted to a queue for execution, but not called from other command buffers. Alternative is Secondary, which cant be submitted directly but can be called from other primary command buffers.
        allocInfo.setCommandBufferCount((uint32_t)m_commandBuffers.size());
        m_commandBuffers = device->GetDevice()->allocateCommandBuffers(allocInfo);
    }

    void Commands::RecordRenderPass(const vulkan::Device* device, const vulkan::RenderPass* renderPass, const vulkan::FrameBuffers* frameBuffers, const vulkan::GraphicsPipeline* pipeline)
    {
        vk::ClearValue const clearValues[1] = { vk::ClearColorValue(std::array<float, 4>({{0.2f, 0.2f, 0.2f, 0.2f}})) };

        // Begin recording on each of the command buffers.
        for (size_t i = 0; i < m_commandBuffers.size(); ++i)
        {
            vk::CommandBufferBeginInfo beginInfo;
            beginInfo.setPInheritanceInfo(nullptr);

            // Begin recording commands to each command buffer.
            m_commandBuffers[i].begin(beginInfo);
            {
                vk::RenderPassBeginInfo renderPassInfo{};
                
                // Specify the render pass and attachments.
                renderPassInfo.setRenderPass(*renderPass->GetRenderPass());
                renderPassInfo.setFramebuffer(*frameBuffers->GetFrameBuffer((uint32_t)i));

                // Specify the dimensions of the render area.
                renderPassInfo.renderArea.setOffset({0,0});
                renderPassInfo.renderArea.setExtent(*device->GetSwapChainExtent());

                // Set clear color
                renderPassInfo.setClearValueCount(1);
                renderPassInfo.setPClearValues(clearValues);

                m_commandBuffers[i].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
                {
                    m_commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline->GetPipeline());
                    m_commandBuffers[i].draw(3, 1, 0, 0); // vertexCount, instanceCount, firstVertex, firstInstance
                }
                m_commandBuffers[i].endRenderPass();
            }
            m_commandBuffers[i].end();
        }
    }

    void Commands::SubmitCommandBuffer(const vulkan::Device* device, uint32_t imageIndex)
    {
        auto currentFrameIndex = device->GetFrameIndex();

        vk::Semaphore waitSemaphores[] = { (*m_renderReadySemaphores)[currentFrameIndex] }; // Which semaphore to wait on before execution begins
        vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput }; // Which stage of the pipeline to wait in
        vk::Semaphore signalSemaphores[] = { (*m_presentReadySemaphores)[currentFrameIndex] }; // Which semaphore to signal when execution completes

        vk::SubmitInfo submitInfo{};
        submitInfo.setWaitSemaphoreCount(1);
        submitInfo.setPWaitSemaphores(waitSemaphores);
        submitInfo.setPWaitDstStageMask(waitStages);
        submitInfo.setCommandBufferCount(1);
        submitInfo.setPCommandBuffers(&m_commandBuffers[imageIndex]);
        submitInfo.setSignalSemaphoreCount(1);
        submitInfo.setPSignalSemaphores(signalSemaphores);

        device->GetQueue(QueueFamilyType::GraphicsFamily)->submit(submitInfo, (*m_framesInFlightFences)[currentFrameIndex]);
    }
}