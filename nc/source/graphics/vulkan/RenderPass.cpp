#include "RenderPass.h"
#include "Device.h"

namespace nc::graphics::vulkan
{
    // @todo: Make this class generic/customizable. This render pass applies to rendering a simple triangle.
    RenderPass::RenderPass(const vulkan::Device* device)
    : m_renderPass{nullptr}
    {
        /***************************
         * COLOR BUFFER ATTACHMENT *
         * *************************/
        vk::AttachmentDescription colorAttachment{};
        colorAttachment.setFormat(*(device->GetSwapChainImageFormat()));
        colorAttachment.setSamples(vk::SampleCountFlagBits::e1); // @todo Multisampling would be set here as well as in the GraphicsPipeline object.
        colorAttachment.setLoadOp(vk::AttachmentLoadOp::eClear); // Sets what to do with this frame buffer before rendering. Currently clears the framebuffer to black before a new frame.
        colorAttachment.setStoreOp(vk::AttachmentStoreOp::eStore); // Sets what to do with this frame buffer after rendering. Currently stores the contents in memory for later reading.
        colorAttachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare); // We dont need the stencil buffer for this basic render, so we set to dont care.
        colorAttachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare); // We dont need the stencil buffer for this basic render, so we set to dont care.
        colorAttachment.setInitialLayout(vk::ImageLayout::eUndefined); // We don't care what layout the image had prior to the render pass beginning.
        colorAttachment.setFinalLayout(vk::ImageLayout::ePresentSrcKHR); // We want the image to be ready for presentation using the swap chain after rendering.

        vk::AttachmentReference colorAttachmentRef{};
        colorAttachmentRef.setAttachment(0); // Reference the attachment by index. ***IMPORTANT*** This index is directly referenced by the shader.
        colorAttachmentRef.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

        /************
         * SUB PASS *
         * **********/
        // Render passes have one or more subpasses. For this simple triangle render, we need only one subpass.
        vk::SubpassDescription subpass{};
        subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics); // Vulkan may support compute subpasses later, so explicitly set this to a graphics bind point.
        subpass.setColorAttachmentCount(1);
        subpass.setPColorAttachments(&colorAttachmentRef);

        /***************
         * RENDER PASS *
         * *************/
        vk::RenderPassCreateInfo renderPassInfo{};
        renderPassInfo.setAttachmentCount(1);
        renderPassInfo.setPAttachments(&colorAttachment);
        renderPassInfo.setSubpassCount(1);
        renderPassInfo.setPSubpasses(&subpass);

        m_renderPass = device->GetDevice()->createRenderPassUnique(renderPassInfo);
    }

    const vk::RenderPass* RenderPass::GetRenderPass() const noexcept
    {
        return &(m_renderPass.get());
    }
}